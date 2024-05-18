#pragma once

#include "DynamicArray.hpp"
#include "OSKmacros.h"

#include <string>

namespace OSK::IO {

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
		static void WriteFile(std::string_view path, const std::string& text);

		/// @brief Escribe un archivo binario.
		/// @param path Ruta del archivo (incluyendo extensión).
		/// @param data Datos (bytes) a escribir).
		static void WriteBinaryFile(std::string_view path, std::span<TByte const> data);

		/// <summary>
		/// Lee un archivo de texto.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>String con el contenido del archivo.</returns>
		/// 
		/// @throws std::runtime_error Si el archivo no existe.
		static std::string ReadFromFile(std::string_view path);

		/// <summary>
		/// Lee un archivo binario.
		/// </summary>
		/// <param name="filename">Ruta del archivo (con extensión).</param>
		/// <returns>Vector de bytes.</returns>
		/// 
		/// @throws std::runtime_error Si el archivo no existe.
		static DynamicArray<char> ReadBinaryFromFile(std::string_view filename);

		/// <summary>
		/// Comprueba si un archivo existe.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>True si existe.</returns>
		static bool FileExists(std::string_view path);

	};

}
