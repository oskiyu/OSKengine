#pragma once

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include "IGraphicsPipeline.h"
#include <string>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GraphicsPipelineDx12 : public IGraphicsPipeline {

	public:

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		ID3D12PipelineState* GetPipelineState() const;
		ID3D12RootSignature* GetLayout() const;

		/// <summary>
		/// Devuelve el tipo de primitiva.
		/// Patch para teselado, triángulos en el 
		/// resto de casos.
		/// </summary>
		D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const;

	private:

		void LoadVertexShader(const std::string& path);
		void LoadFragmentShader(const std::string& path);
		void LoadTeselationControlShader(const std::string& path);
		void LoadTesselationEvaluationShader(const std::string& path);

		D3D12_RASTERIZER_DESC GetRasterizerDesc(const PipelineCreateInfo& info) const;
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(const PipelineCreateInfo& info) const;
		D3D12_BLEND_DESC GetBlendDesc(const PipelineCreateInfo& info) const;

		//Patch para teselado, triángulos en el 
		// resto de casos.
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		ComPtr<ID3D12PipelineState> pipeline;

		ComPtr<ID3DBlob> vertexShader;
		D3D12_SHADER_BYTECODE vertexShaderBytecode{};

		ComPtr<ID3DBlob> fragmentShader;
		D3D12_SHADER_BYTECODE fragmentShaderBytecode{};

		ComPtr<ID3DBlob> tesselationControlShader;
		D3D12_SHADER_BYTECODE tesselationControlShaderBytecode{};

		ComPtr<ID3DBlob> tesselationEvaluationShader;
		D3D12_SHADER_BYTECODE tesselationEvaluationShaderBytecode{};

	};

}
