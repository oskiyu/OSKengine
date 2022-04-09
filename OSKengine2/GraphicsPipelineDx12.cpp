#include "GraphicsPipelineDx12.h"

#include "VertexDx12.h"
#include "GpuDx12.h"
#include <string>
#include <d3dcompiler.h>
#include "Assert.h"
#include "PipelineCreateInfo.h"
#include "PipelineLayoutDx12.h"
#include "Format.h"
#include "FormatDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

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

std::wstring s2ws(const std::string& s) {
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

ComPtr<ID3DBlob> LoadBlob(LPCWSTR filename) {
	HRESULT hr{};

	ComPtr<ID3DBlob> shaderBlob; // chunk of memory

	hr = D3DReadFileToBlob(filename, &shaderBlob);

	if (FAILED(hr))
		OSK_ASSERT(false, "No se pudo compilar el shader. Code: " + std::to_string(hr));

	return shaderBlob;
}


void GraphicsPipelineDx12::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info) {
	layout = new PipelineLayoutDx12(materialLayout);
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC createInfo{};
	createInfo.InputLayout = GetInputLayoutDescDx12_Vertex3D();

	LoadVertexShader(info.vertexPath);
	LoadFragmentShader(info.fragmentPath);

	createInfo.VS = vertexShaderBytecode;
	createInfo.PS = fragmentShaderBytecode;

	createInfo.RasterizerState = GetRasterizerDesc(info);
	createInfo.BlendState = GetBlendDesc(info);
	createInfo.DepthStencilState = GetDepthStencilDesc(info);
	createInfo.SampleMask = UINT_MAX;
	createInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	createInfo.NumRenderTargets = 1;
	createInfo.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	createInfo.DSVFormat = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
	createInfo.SampleDesc.Count = 1;

	createInfo.pRootSignature = layout->As<PipelineLayoutDx12>()->GetSignature();

	device->As<GpuDx12>()->GetDevice()->CreateGraphicsPipelineState(&createInfo, IID_PPV_ARGS(&pipeline));
}

ID3D12PipelineState* GraphicsPipelineDx12::GetPipelineState() const {
	return pipeline.Get();
}

ID3D12RootSignature* GraphicsPipelineDx12::GetLayout() const {
	return layout->As<PipelineLayoutDx12>()->GetSignature();
}

void GraphicsPipelineDx12::LoadVertexShader(const std::string& path) {
	vertexShader = LoadBlob(s2ws(path).c_str());

	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
}

void GraphicsPipelineDx12::LoadFragmentShader(const std::string& path) {
	fragmentShader = LoadBlob(s2ws(path).c_str());

	fragmentShaderBytecode.pShaderBytecode = fragmentShader->GetBufferPointer();
	fragmentShaderBytecode.BytecodeLength = fragmentShader->GetBufferSize();
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
	  FALSE,FALSE,
	  D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
	  D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
	  D3D12_LOGIC_OP_NOOP,
	  D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	for (TSize i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		desc.RenderTarget[i] = DefaultRenderTargetBlendDesc;

	return desc;
}
