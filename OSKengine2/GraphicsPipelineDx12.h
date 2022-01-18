#pragma once

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include "IGraphicsPipeline.h"
#include <string>

namespace OSK {

	class OSKAPI_CALL GraphicsPipelineDx12 : public IGraphicsPipeline {

	public:

		void Create(IGpu* device, const PipelineCreateInfo& info) override;

		ID3D12PipelineState* GetPipelineState() const;
		ID3D12RootSignature* GetLayout() const;

	private:

		void LoadVertexShader(const std::string& path);
		void LoadFragmentShader(const std::string& path);

		void SetLayout(IGpu* device);

		D3D12_RASTERIZER_DESC GetRasterizerDesc(const PipelineCreateInfo& info) const;
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(const PipelineCreateInfo& info) const;
		D3D12_BLEND_DESC GetBlendDesc(const PipelineCreateInfo& info) const;

		ComPtr<ID3D12PipelineState> pipeline;

		ComPtr<ID3DBlob> vertexShader;
		D3D12_SHADER_BYTECODE vertexShaderBytecode;

		ComPtr<ID3DBlob> fragmentShader;
		D3D12_SHADER_BYTECODE fragmentShaderBytecode;

		ComPtr<ID3D12RootSignature> layout;

	};

}
