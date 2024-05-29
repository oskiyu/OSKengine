#pragma once

#include "ApiCall.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"
#include "PipelineCreateInfo.h"

#include <string>
#include "HlslRuntimeCompiler.h"

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxcapi.h>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Contiene los datos compilados de un shader.
	/// </summary>
	struct ShaderStageDx12 {
		
		/// <summary>
		/// Blob de datos con el shader compilado.
		/// </summary>
		/// 
		/// @pre Únicamente válido si el shader ha sido
		/// cargado desde un archivo precompilado.
		/// @note Si no fue cargado desde un archivo precompilado,
		/// será nullptr.
		ComPtr<ID3DBlob> precompiled;

		/// <summary>
		/// Blob de datos con el shader compilado.
		/// </summary>
		/// 
		/// @pre Únicamente válido si el shader ha sido
		/// compilado a partir de un shader SPIR-V.
		/// @note Si no fue cargado desde un archivo precompilado,
		/// será nullptr.
		ComPtr<IDxcBlob> spirvCompiled;

		bool isBlobPrecompiled = false;
		D3D12_SHADER_BYTECODE bytecode{};

	};


	/// <summary>
	/// Clase base para todos los pipelines de DirectX 12.
	/// </summary>
	class OSKAPI_CALL IPipelineDx12 {

	public:

		virtual ~IPipelineDx12() = default;

		/// <summary>
		/// Devuelve el pipeline nativo.
		/// </summary>
		ID3D12PipelineState* GetPipeline() const;

	protected:

		/// <summary>
		/// Compila un shader.
		/// </summary>
		/// <param name="filename">Path del archivo.</param>
		/// 
		/// @pre El archivo apuntado por filename debe estar precompilado.
		static ComPtr<ID3DBlob> LoadBlob(LPCWSTR filename);

		/// <summary>
		/// Compila un shader.
		/// </summary>
		/// <param name="spirvPath">Path del archivo SPIR-V.</param>
		/// <param name="vertexInfo">Información del vértice asociado al shader.</param>
		/// <param name="hlslProfile">Perfil para la compilación HLSL.</param>
		/// <param name="layout">Layout del shader.</param>
		/// 
		/// @pre El archivo apuntado por spirvPath debe ser un archivo
		/// SPIR-V válido.
		/// @pre Si el shader stage es ShaderStage::VERTEX, vertexInfo debe ser una estructura válida.
		/// 
		/// @throws ShaderCompilingException Si ocurre algún error.
		static ComPtr<IDxcBlob> CompileBlob(const std::string& spirvPath, const VertexInfo& vertexInfo, const std::string& hlslProfile, const MaterialLayout& layout);

		/// @throws ShaderCompilingException Si ocurre algún error.
		ShaderStageDx12 LoadShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const std::string& path, ShaderStage stage, const MaterialLayout& mLayout) const;

		ComPtr<ID3D12PipelineState> dxPipeline;

	private:

		static std::string GetHlslProfile(ShaderStage stage);

	};

}
