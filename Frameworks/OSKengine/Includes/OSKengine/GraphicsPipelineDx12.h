#pragma once

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxc/dxcapi.h>

#include "IGraphicsPipeline.h"
#include <string>

namespace OSK::GRAPHICS {

	struct PipelineCreateInfo;

	class OSKAPI_CALL GraphicsPipelineDx12 : public IGraphicsPipeline {

	public:

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		ID3D12PipelineState* GetPipelineState() const;
		ID3D12RootSignature* GetLayout() const;

		/// <summary>
		/// Devuelve el tipo de primitiva.
		/// Patch para teselado, tri�ngulos en el 
		/// resto de casos.
		/// </summary>
		D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const;

	private:

		void LoadVertexShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout);
		void LoadFragmentShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout);
		void LoadTeselationControlShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout);
		void LoadTesselationEvaluationShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const MaterialLayout& layout);

		D3D12_RASTERIZER_DESC GetRasterizerDesc(const PipelineCreateInfo& info) const;
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(const PipelineCreateInfo& info) const;
		D3D12_BLEND_DESC GetBlendDesc(const PipelineCreateInfo& info) const;

		//Patch para teselado, tri�ngulos en el 
		// resto de casos.
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		ComPtr<ID3D12PipelineState> pipeline;

		// Los shaders precompilados se cargan en un ID3DBlob,
		// mientras que los compilados a partir de SPIR-V se cargan
		// en IDxcBlob.
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<IDxcBlob> vertexShader2;
		D3D12_SHADER_BYTECODE vertexShaderBytecode{};

		ComPtr<ID3DBlob> fragmentShader;
		ComPtr<IDxcBlob> fragmentShader2;
		D3D12_SHADER_BYTECODE fragmentShaderBytecode{};

		ComPtr<ID3DBlob> tesselationControlShader;
		ComPtr<IDxcBlob> tesselationControlShader2;
		D3D12_SHADER_BYTECODE tesselationControlShaderBytecode{};

		ComPtr<ID3DBlob> tesselationEvaluationShader;
		ComPtr<IDxcBlob> tesselationEvaluationShader2;
		D3D12_SHADER_BYTECODE tesselationEvaluationShaderBytecode{};

	};

}