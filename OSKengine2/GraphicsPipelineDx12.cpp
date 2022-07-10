#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

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
#include "LinkedList.hpp"
#include "MaterialLayout.h"

#include "FileIO.h"

#include "OSKengine.h"
#include "Logger.h"

#include "WindowsUtils.h"

#include "SpirvToHlsl.h"
#include "HlslRuntimeCompiler.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;

DXGI_FORMAT GetVertexAttribFormatDx12(const VertexInfo::Entry& entry) {
	switch (entry.type) {

	case VertexInfo::Entry::Type::INT:
		if (entry.size == 2 * sizeof(int)) return DXGI_FORMAT_R32G32_SINT;
		if (entry.size == 3 * sizeof(int)) return DXGI_FORMAT_R32G32B32_SINT;
		if (entry.size == 4 * sizeof(int)) return DXGI_FORMAT_R32G32B32A32_SINT;

	case VertexInfo::Entry::Type::FLOAT:
		if (entry.size == 2 * sizeof(float)) return DXGI_FORMAT_R32G32_FLOAT;
		if (entry.size == 3 * sizeof(float)) return DXGI_FORMAT_R32G32B32_FLOAT;
		if (entry.size == 4 * sizeof(float)) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	OSK_ASSERT(false, "Formato de vértice incorecto.");
	return DXGI_FORMAT_UNKNOWN;
}

DynamicArray<D3D12_INPUT_ELEMENT_DESC> GetInputLayoutDescDx12(const VertexInfo& info) {
	DynamicArray<D3D12_INPUT_ELEMENT_DESC> output{};

	TSize offset = 0;
	for (TSize i = 0; i < info.entries.GetSize(); i++) {
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


D3D12_CULL_MODE GetCullMode(PolygonCullMode mode) {
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

D3D12_FILL_MODE GetFillMode(PolygonMode mode) {
	switch (mode) {
	case PolygonMode::FILL:
		return D3D12_FILL_MODE_SOLID;
	case PolygonMode::LINE:
		return D3D12_FILL_MODE_WIREFRAME;
	default:
		return D3D12_FILL_MODE_SOLID;
	}
}

ComPtr<ID3DBlob> LoadBlob(LPCWSTR filename) {
	HRESULT hr{};

	ComPtr<ID3DBlob> shaderBlob; // chunk of memory

	hr = D3DReadFileToBlob(filename, &shaderBlob);

	if (FAILED(hr))
		OSK_ASSERT(false, "No se pudo compilar el shader. Code: " + std::to_string(hr));

	return shaderBlob;
}

ComPtr<IDxcBlob> CompileShaderSpv(const std::string& spirvPath, const VertexInfo& vertexInfo, const std::string& hlslProfile, const MaterialLayout& layout) {
	const auto bytecode = FileIO::ReadBinaryFromFile(spirvPath);
	
	// Generación del código HLSL.
	SpirvToHlsl compilerToHlsl = SpirvToHlsl(bytecode);
	compilerToHlsl.SetHlslTargetProfile(6, 1);
	compilerToHlsl.SetVertexAttributesMapping(vertexInfo);
	compilerToHlsl.SetLayoutMapping(layout);

	const std::string hlslSourceCode = compilerToHlsl.CreateHlsl();

	//Engine::GetLogger()->DebugLog(hlslSourceCode);
	FileIO::WriteFile("./temp_shader", hlslSourceCode);

	HlslRuntimeCompiler compiler;
	return compiler.CompileFromFile("./temp_shader", hlslProfile);
}

void GraphicsPipelineDx12::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info, Format targetImageFormat, const VertexInfo& vertexInfo) {
	layout = new PipelineLayoutDx12(materialLayout);
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC createInfo{};

	auto layoutDesc = GetInputLayoutDescDx12(vertexInfo);

	for (TSize i = 0; i < layoutDesc.GetSize(); i++)
		layoutDesc.At(i).SemanticName = vertexInfo.entries[i].GetName().c_str();

	D3D12_INPUT_LAYOUT_DESC inputLayout{};
	inputLayout.NumElements = layoutDesc.GetSize();
	inputLayout.pInputElementDescs = layoutDesc.GetData();

	createInfo.InputLayout = inputLayout;

	LoadVertexShader(info, vertexInfo, *materialLayout);
	LoadFragmentShader(info, vertexInfo, *materialLayout);

	if (info.tesselationControlPath != "") {
		LoadTeselationControlShader(info, vertexInfo, *materialLayout);
		createInfo.HS = tesselationControlShaderBytecode;

		topologyType = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	}
	if (info.tesselationEvaluationPath != "") {
		LoadTesselationEvaluationShader(info, vertexInfo, *materialLayout);
		createInfo.DS = tesselationEvaluationShaderBytecode;
	}

	createInfo.VS = vertexShaderBytecode;
	createInfo.PS = fragmentShaderBytecode;

	createInfo.RasterizerState = GetRasterizerDesc(info);
	createInfo.BlendState = GetBlendDesc(info);
	createInfo.DepthStencilState = GetDepthStencilDesc(info);
	createInfo.SampleMask = UINT_MAX;
	if (info.tesselationControlPath != "")
		createInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	else
		createInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	createInfo.NumRenderTargets = 1;
	createInfo.RTVFormats[0] = GetFormatDx12(targetImageFormat);
	createInfo.DSVFormat = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
	createInfo.SampleDesc.Count = 1;

	createInfo.pRootSignature = layout->As<PipelineLayoutDx12>()->GetSignature();

	HRESULT result = device->As<GpuDx12>()->GetDevice()->CreateGraphicsPipelineState(&createInfo, IID_PPV_ARGS(&pipeline));
	OSK_ASSERT(SUCCEEDED(result), "Error al crear el pipeline. Code: " + std::to_string(result));
}

ID3D12PipelineState* GraphicsPipelineDx12::GetPipelineState() const {
	return pipeline.Get();
}

ID3D12RootSignature* GraphicsPipelineDx12::GetLayout() const {
	return layout->As<PipelineLayoutDx12>()->GetSignature();
}

void GraphicsPipelineDx12::LoadVertexShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout) {
	if (info.precompiledHlslShaders) {
		vertexShader = LoadBlob(StringToWideString(info.vertexPath).c_str());

		vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();
		vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	}
	else {
		vertexShader2 = CompileShaderSpv(info.vertexPath, vertexInfo, "vs_6_1", layout);

		vertexShaderBytecode.pShaderBytecode = vertexShader2->GetBufferPointer();
		vertexShaderBytecode.BytecodeLength = vertexShader2->GetBufferSize();
	}
}

void GraphicsPipelineDx12::LoadFragmentShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout) {
	if (info.precompiledHlslShaders) {
		fragmentShader = LoadBlob(StringToWideString(info.fragmentPath).c_str());

		fragmentShaderBytecode.pShaderBytecode = fragmentShader->GetBufferPointer();
		fragmentShaderBytecode.BytecodeLength = fragmentShader->GetBufferSize();
	}
	else {
		fragmentShader2 = CompileShaderSpv(info.fragmentPath, vertexInfo, "ps_6_1", layout);

		fragmentShaderBytecode.pShaderBytecode = fragmentShader2->GetBufferPointer();
		fragmentShaderBytecode.BytecodeLength = fragmentShader2->GetBufferSize();
	}
}

void GraphicsPipelineDx12::LoadTeselationControlShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout) {
	if (info.precompiledHlslShaders) {
		tesselationControlShader = LoadBlob(StringToWideString(info.tesselationControlPath).c_str());

		tesselationControlShaderBytecode.pShaderBytecode = tesselationControlShader->GetBufferPointer();
		tesselationControlShaderBytecode.BytecodeLength = tesselationControlShader->GetBufferSize();
	}
	else {
		tesselationControlShader2 = CompileShaderSpv(info.tesselationControlPath, vertexInfo, "hs_6_1", layout);

		tesselationControlShaderBytecode.pShaderBytecode = tesselationControlShader2->GetBufferPointer();
		tesselationControlShaderBytecode.BytecodeLength = tesselationControlShader2->GetBufferSize();
	}
}

void GraphicsPipelineDx12::LoadTesselationEvaluationShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout) {
	if (info.precompiledHlslShaders) {
		tesselationEvaluationShader = LoadBlob(StringToWideString(info.tesselationEvaluationPath).c_str());

		tesselationEvaluationShaderBytecode.pShaderBytecode = tesselationEvaluationShader->GetBufferPointer();
		tesselationEvaluationShaderBytecode.BytecodeLength = tesselationEvaluationShader->GetBufferSize();
	}
	else {
		tesselationEvaluationShader2 = CompileShaderSpv(info.tesselationEvaluationPath, vertexInfo, "ds_6_1", layout);

		tesselationEvaluationShaderBytecode.pShaderBytecode = tesselationEvaluationShader2->GetBufferPointer();
		tesselationEvaluationShaderBytecode.BytecodeLength = tesselationEvaluationShader2->GetBufferSize();
	}
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

	for (TSize i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		desc.RenderTarget[i] = DefaultRenderTargetBlendDesc;

	return desc;
}
