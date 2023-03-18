#pragma once

#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxc/dxcapi.h>

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Clase auxiliar que permite compilar shaders HLSL en
	/// tiempo de ejecución.
	/// </summary>
	class HlslRuntimeCompiler {

	public:

		/// <summary>
		/// Inicializa los componentes globales.
		/// </summary>
		static void InitializeComponents();

		/// <summary>
		/// Compila el código HLSL almacenado en el archivo indicado.
		/// </summary>
		/// <param name="path">Ruta del archivo HLSL.</param>
		/// <param name="hlslProfile">Perfil de compilación.</param>
		/// <returns>Blob con el shader compilado.</returns>
		/// 
		/// @pre Se debe haber llamado a HlslRuntimeCompiler::InitializeComponents.
		ComPtr<IDxcBlob> CompileFromFile(const std::string& path, const std::string& hlslProfile);

	private:

		static ComPtr<IDxcCompiler3> compiler;
		static ComPtr<IDxcUtils> utils;
		static ComPtr<IDxcIncludeHandler> includeHandler;

	};

}
