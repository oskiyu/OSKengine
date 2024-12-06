#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxc/dxcapi.h>

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Clase auxiliar que permite compilar shaders HLSL en
	/// tiempo de ejecuci�n.
	/// </summary>
	class HlslRuntimeCompiler {

	public:

		/// <summary>
		/// Inicializa los componentes globales.
		/// </summary>
		/// 
		/// @throws ShaderCompilingException Si ocurre alg�n error.
		static void InitializeComponents();

		/// <summary>
		/// Compila el c�digo HLSL almacenado en el archivo indicado.
		/// </summary>
		/// <param name="path">Ruta del archivo HLSL.</param>
		/// <param name="hlslProfile">Perfil de compilaci�n.</param>
		/// <returns>Blob con el shader compilado.</returns>
		/// 
		/// @pre Se debe haber llamado a HlslRuntimeCompiler::InitializeComponents.
		/// 
		/// @throws ShaderCompilingException Si ocurre alg�n error.
		ComPtr<IDxcBlob> CompileFromFile(const std::string& path, const std::string& hlslProfile);

	private:

		static ComPtr<IDxcCompiler3> compiler;
		static ComPtr<IDxcUtils> utils;
		static ComPtr<IDxcIncludeHandler> includeHandler;

	};

}

#endif
