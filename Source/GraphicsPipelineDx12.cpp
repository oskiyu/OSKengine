#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

#include "SpirvToHlsl.h"
#include "HlslRuntimeCompiler.h"

#include "GraphicsPipelineDx12.h"

#include "GpuDx12.h"
#include <string>
#include <d3dcompiler.h>
#include "Assert.h"
#include "PipelineCreateInfo.h"
#include "PipelineLayoutDx12.h"
#include "Format.h"
#include "FormatDx12.h"
#include "VertexInfo.h"
#include "MaterialLayout.h"
#include "PipelinesExceptions.h"

#include "FileIO.h"
#include "MathExceptions.h"

#include "OSKengine.h"
#include "Logger.h"

#include "StringOperations.h"


using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;


void GraphicsPipelineDx12::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info,  const VertexInfo& vertexInfo) {
	layout = new PipelineLayoutDx12(materialLayout);
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC createInfo{};

	auto layoutDesc = GetInputLayoutDescDx12(vertexInfo);

	for (UIndex32 i = 0; i < layoutDesc.GetSize(); i++)
		layoutDesc.At(i).SemanticName = vertexInfo.entries[i].GetName().c_str();

	OSK_ASSERT(layoutDesc.GetSize() < std::numeric_limits<UINT>::max(), OverflowConversionException());
	OSK_ASSERT(info.formats.GetSize() < std::numeric_limits<UINT>::max(), OverflowConversionException());

	D3D12_INPUT_LAYOUT_DESC inputLayout{};
	inputLayout.NumElements = static_cast<UINT>(layoutDesc.GetSize());
	inputLayout.pInputElementDescs = layoutDesc.GetData();

	createInfo.InputLayout = inputLayout;

	vertexShader = LoadShader(info, vertexInfo, info.vertexPath, ShaderStage::VERTEX, *materialLayout);
	fragmentShader = LoadShader(info, vertexInfo, info.fragmentPath, ShaderStage::FRAGMENT, *materialLayout);

	createInfo.VS = vertexShader.bytecode;
	createInfo.PS = fragmentShader.bytecode;

	if (info.tesselationControlPath != "") {
		tesselationControlShader = LoadShader(info, vertexInfo, info.tesselationControlPath, ShaderStage::TESSELATION_CONTROL, *materialLayout);
		createInfo.HS = tesselationControlShader.bytecode;

		topologyType = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	}
	if (info.tesselationEvaluationPath != "") {
		tesselationEvaluationShader = LoadShader(info, vertexInfo, info.tesselationEvaluationPath, ShaderStage::TESSELATION_EVALUATION, *materialLayout);
		createInfo.DS = tesselationEvaluationShader.bytecode;
	}

	createInfo.RasterizerState = GetRasterizerDesc(info);
	createInfo.BlendState = GetBlendDesc(info);
	createInfo.DepthStencilState = GetDepthStencilDesc(info);
	createInfo.SampleMask = UINT_MAX;
	if (info.tesselationControlPath != "")
		createInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	else
		createInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	createInfo.NumRenderTargets = static_cast<UINT>(info.formats.GetSize());
	for (UIndex32 i = 0; i < info.formats.GetSize(); i++)
		createInfo.RTVFormats[i] = GetFormatDx12(info.formats[i]);

	createInfo.DSVFormat = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
	createInfo.SampleDesc.Count = 1;

	createInfo.pRootSignature = layout->As<PipelineLayoutDx12>()->GetSignature();

	HRESULT result = device->As<GpuDx12>()->GetDevice()->CreateGraphicsPipelineState(&createInfo, IID_PPV_ARGS(&dxPipeline));
	OSK_ASSERT(SUCCEEDED(result), PipelineCreationException(result));
}

void GraphicsPipelineDx12::SetDebugName(const std::string& name) {
	dxPipeline->SetName(StringToWideString(name).c_str());
}

D3D_PRIMITIVE_TOPOLOGY GraphicsPipelineDx12::GetTopologyType() const {
	return topologyType;
}

D3D12_RASTERIZER_DESC GraphicsPipelineDx12::GetRasterizerDesc(const PipelineCreateInfo& info) const {
	D3D12_RASTERIZER_DESC desc{};
	desc.FillMode = GetFillMode(info.polygonMode);
	desc.CullMode = GetCullMode(info.cullMode);
	desc.FrontCounterClockwise = info.frontFaceType == PolygonFrontFaceType::COUNTERCLOCKWISE;
	desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	desc.ForcedSampleCount = 0;
	desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return desc;
}

D3D12_DEPTH_STENCIL_DESC GraphicsPipelineDx12::GetDepthStencilDesc(const PipelineCreateInfo& info) const {
	D3D12_DEPTH_STENCIL_DESC desc{};

	if (info.depthTestingType == DepthTestingType::NONE)
		desc.DepthEnable = FALSE;
	else
		desc.DepthEnable = TRUE;

	if (info.depthTestingType == DepthTestingType::READ_WRITE)
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	else
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	desc.StencilEnable = FALSE;
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =  { 
		D3D12_STENCIL_OP_KEEP, 
		D3D12_STENCIL_OP_KEEP, 
		D3D12_STENCIL_OP_KEEP, 
		D3D12_COMPARISON_FUNC_ALWAYS 
	};

	desc.BackFace = defaultStencilOp;
	desc.FrontFace = defaultStencilOp;

	return desc;
}

D3D12_BLEND_DESC GraphicsPipelineDx12::GetBlendDesc(const PipelineCreateInfo& info) const {
	D3D12_BLEND_DESC desc{};
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	const static D3D12_RENDER_TARGET_BLEND_DESC DefaultRenderTargetBlendDesc = {
	  TRUE, FALSE,
	  D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
	  D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
	  D3D12_LOGIC_OP_NOOP,
	  D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	for (UIndex32 i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		desc.RenderTarget[i] = DefaultRenderTargetBlendDesc;

	return desc;
}

DXGI_FORMAT GraphicsPipelineDx12::GetVertexAttribFormatDx12(const VertexInfo::Entry& entry) const {
	switch (entry.type) {

	case VertexInfo::Entry::Type::INT:
		if (entry.size == 2 * sizeof(int)) return DXGI_FORMAT_R32G32_SINT;
		if (entry.size == 3 * sizeof(int)) return DXGI_FORMAT_R32G32B32_SINT;
		if (entry.size == 4 * sizeof(int)) return DXGI_FORMAT_R32G32B32A32_SINT;
		break;

	case VertexInfo::Entry::Type::FLOAT:
		if (entry.size == 2 * sizeof(float)) return DXGI_FORMAT_R32G32_FLOAT;
		if (entry.size == 3 * sizeof(float)) return DXGI_FORMAT_R32G32B32_FLOAT;
		if (entry.size == 4 * sizeof(float)) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	}

	OSK_ASSERT(false, NotImplementedException());
	return DXGI_FORMAT_UNKNOWN;
}

DynamicArray<D3D12_INPUT_ELEMENT_DESC> GraphicsPipelineDx12::GetInputLayoutDescDx12(const VertexInfo& info) const {
	DynamicArray<D3D12_INPUT_ELEMENT_DESC> output{};

	USize64 offset = 0;
	for (UIndex32 i = 0; i < info.entries.GetSize(); i++) {
		const VertexInfo::Entry& entry = info.entries.At(i);

		D3D12_INPUT_ELEMENT_DESC desc = {
			NULL,
			0,
			GetVertexAttribFormatDx12(entry),
			0,
			offset,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		output.Insert(desc);

		offset += entry.size;
	}

	return output;
}

D3D12_CULL_MODE GraphicsPipelineDx12::GetCullMode(PolygonCullMode mode) const {
	switch (mode) {
	case PolygonCullMode::FRONT:
		return D3D12_CULL_MODE_FRONT;
	case PolygonCullMode::BACK:
		return D3D12_CULL_MODE_BACK;
	case PolygonCullMode::NONE:
		return D3D12_CULL_MODE_NONE;
	default:
		return D3D12_CULL_MODE_NONE;
	}
}

D3D12_FILL_MODE GraphicsPipelineDx12::GetFillMode(PolygonMode mode) const {
	switch (mode) {
	case PolygonMode::TRIANGLE_FILL:
		return D3D12_FILL_MODE_SOLID;
	case PolygonMode::TRIANGLE_WIDEFRAME:
		return D3D12_FILL_MODE_WIREFRAME;
	case PolygonMode::LINE:
		return D3D12_FILL_MODE_WIREFRAME;
	default:
		return D3D12_FILL_MODE_SOLID;
	}
}

#endif
