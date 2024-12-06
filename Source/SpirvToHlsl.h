#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "ApiCall.h"

#include <string>

#include "DynamicArray.hpp"

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

namespace OSK::GRAPHICS {

	class VertexInfo;
	class MaterialLayout;

	/// <summary>
	/// Clase auxiliar que genera código HLSL a partir de código
	/// SPIR-V, usando spirv_cross.
	/// </summary>
	class SpirvToHlsl {

	public:

		/// <summary>
		/// Inicializa el compilador SPIRV-HLSL con el bytecode
		/// SPIR-V dado.
		/// </summary>
		/// <param name="spirvBytecode">Bytecode SPIRV válido.</param>
		SpirvToHlsl(const DynamicArray<char>& spirvBytecode);

		/// <summary>
		/// Establece los nombres semánticos que tendrán los
		/// atributos de los vértices en el shader.
		/// </summary>
		void SetVertexAttributesMapping(const VertexInfo& vertexInfo);

		/// <summary>
		/// Se establece qué registros ocupará cada
		/// </summary>
		/// <param name="layout"></param>
		void SetLayoutMapping(const MaterialLayout& layout);

		/// <summary>
		/// Establece el perfil de HLSL generado.
		/// Por defecto usa 6.1.
		/// </summary>
		/// 
		/// @warning No comprueba si es un perfil válido.
		void SetHlslTargetProfile(USize64 mayor, USize64 minor);

		/// <summary>
		/// Devuelve el código HLSL.
		/// </summary>
		/// 
		/// @pre Se ha llamado a SpirvToHlsl::SetVertexAttributesMapping.
		/// @pre Se ha llamado a SpirvToHlsl::SetLayoutMapping.
		/// @pre Se ha llamado a SpirvToHlsl::SetHlslTargetProfile.
		std::string CreateHlsl();

	private:

		spirv_cross::CompilerHLSL compilerToHlsl;

	};

}

#endif
