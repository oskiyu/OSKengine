#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <fstream>
#include <vector>

namespace OSK {

	/// <summary>
	/// Clase que actúa de interfaz para la lectura y escritura de atchivos.
	/// </summary>
	class OSKAPI_CALL FileIO {
	
	public:

		/// <summary>
		/// Escribe un archivo de texto.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <param name="text">Texto que se va a escribir.</param>
		static void WriteFile(const std::string& path, const std::string& text);

		/// <summary>
		/// Lee un archivo de texto.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>String con el contenido del archivo.</returns>
		static std::string ReadFromFile(const std::string& path);

		/// <summary>
		/// Lee un archivo binario.
		/// </summary>
		/// <param name="filename">Ruta del archivo (con extensión).</param>
		/// <returns>Vector de bytes.</returns>
		static std::vector<char> ReadBinaryFromFile(const std::string& filename);

		/// <summary>
		/// Comprueba si un archivo existe.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>True si existe.</returns>
		static bool FileExists(const std::string& path);

	};

}