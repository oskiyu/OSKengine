#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IGraphicsPipeline.h"
#include "PipelineCreateInfo.h"
#include "DynamicArray.hpp"
#include "VertexInfo.h"
#include "IPipelineDx12.h"

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxcapi.h>

#include <string>

namespace OSK::GRAPHICS {

	struct PipelineCreateInfo;

	class OSKAPI_CALL GraphicsPipelineDx12 : public IGraphicsPipeline, public IPipelineDx12 {

	public:

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		void SetDebugName(const std::string& name) override;

		/// <summary>
		/// Devuelve el tipo de primitiva.
		/// Patch para teselado, triángulos en el 
		/// resto de casos.
		/// </summary>
		D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const;

	private:

		D3D12_RASTERIZER_DESC GetRasterizerDesc(const PipelineCreateInfo& info) const;
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(const PipelineCreateInfo& info) const;
		D3D12_BLEND_DESC GetBlendDesc(const PipelineCreateInfo& info) const;
		DXGI_FORMAT GetVertexAttribFormatDx12(const VertexInfo::Entry& entry) const;
		DynamicArray<D3D12_INPUT_ELEMENT_DESC> GetInputLayoutDescDx12(const VertexInfo& info) const;
		D3D12_CULL_MODE GetCullMode(PolygonCullMode mode) const;
		D3D12_FILL_MODE GetFillMode(PolygonMode mode) const;

		//Patch para teselado, triángulos en el 
		// resto de casos.
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// Los shaders precompilados se cargan en un ID3DBlob,
		// mientras que los compilados a partir de SPIR-V se cargan
		// en IDxcBlob.

		ShaderStageDx12 vertexShader{};
		ShaderStageDx12 fragmentShader{};
		ShaderStageDx12 tesselationControlShader{};
		ShaderStageDx12 tesselationEvaluationShader{};

	};

}

#endif
