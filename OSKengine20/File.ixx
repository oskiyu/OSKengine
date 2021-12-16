#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Assert.h"

export module OSKengine.File;

import OSKengine.Assert;

export namespace OSK {

	/// <summary>
	/// Flags para determinar el tipo de archivo que se va a leer/escribir.
	/// </summary>
	enum class FileOpenType : int {
		/// <summary>
		/// Abrir para lectura.
		/// </summary>
		READ = 1 << 0,
		/// <summary>
		/// Abrir para escritura.
		/// </summary>
		WRITE = 1 << 1,
		/// <summary>
		/// Abrir en modo binario.
		/// </summary>
		BINARY = 1 << 2,
		/// <summary>
		/// Se sobreescribe su contenido al escribir.
		/// </summary>
		OVERRIDE
	};

	/// <summary>
	/// Clase que permite acceder (leer o escribir) archivos.
	/// 
	/// Para poder usarse primero debe abrirse.
	/// </summary>
	class File {

	public:

		/// <summary>
		/// Cierra el archivo.
		/// </summary>
		~File() {
			if (IsOpen())
				Close();
		}

		/// <summary>
		/// Abre un archivo.
		/// </summary>
		/// <param name="path">Ruta del archivo.</param>
		/// <param name="flags">Flags que determinan el tipo de archivo y si se abre para lectura/escritura.</param>
		void Open(const std::string& path, int flags) {
			if (flags & (int)FileOpenType::READ)
				OSK_ASSERT(FileExists(path), "Se ha intentado abrir el archivo " + path + " pero no existe.");

			this->flags = flags;

			int fFlags = std::ios::ate;

			// Se abre desde el principio.
			if (flags & (int)FileOpenType::OVERRIDE)
				fFlags = 0;

			// Archivo binario.
			if (flags & (int)FileOpenType::BINARY)
				fFlags = fFlags | std::ios::binary;

			// Lectura.
			if (flags & (int)FileOpenType::READ)
				inputStream = std::ifstream(path, fFlags);

			// Escritura.
			if (flags & (int)FileOpenType::WRITE)
				outputStream = std::ofstream(path, fFlags);
		}

		/// <summary>
		/// Cierra el archivo abierto.
		/// </summary>
		void Close() {
			OSK_ASSERT(IsOpen(), "No se puede cerrar un archivo que no ha sido abierto.");

			if (inputStream.is_open())
				inputStream.close();

			if (outputStream.is_open())
				outputStream.close();
		}

		/// <summary>
		/// Escribe el texto.
		/// </summary>
		void Write(const std::string& text) {
			OSK_ASSERT(IsWriteOpen(), "No se puede escribir en un archivo que no ha sido abierto.");

			outputStream << text;
		}

		/// <summary>
		/// Escribe una línea.
		/// </summary>
		void WriteLine(const std::string& line) {
			OSK_ASSERT(IsWriteOpen(), "No se puede escribir en un archivo que no ha sido abierto.");

			outputStream << line << std::endl;
		}

		/// <summary>
		/// Lee la siguiente línea del archivo.
		/// </summary>
		std::string ReadLine() {
			OSK_ASSERT(IsReadOpen(), "No se puede leer en un archivo que no ha sido abierto.");

			std::string line;
			std::getline(inputStream, line);

			return line;
		}

		/// <summary>
		/// Lee todo el contenido del archivo.
		/// </summary>
		std::string ReadAll() {
			OSK_ASSERT(IsReadOpen(), "No se puede leer en un archivo que no ha sido abierto.");

			std::string line;
			std::string output;

			while (std::getline(inputStream, line)) {
				output.append(line);
				output.append("\n");
			}

			return output;
		}

		/// <summary>
		/// Lee todo el contenido del archivo, en modo binario.
		/// </summary>
		std::vector<char> ReadAllBinary() {
			OSK_ASSERT(IsReadOpen(), "No se puede leer en un archivo que no ha sido abierto.");
			OSK_ASSERT(IsBinaryOpen(), "No se puede leer en un archivo que no ha sido abierto.");

			size_t fileSize = (size_t)inputStream.tellg();
			std::vector<char> buffer(fileSize);

			inputStream.seekg(0);
			inputStream.read(buffer.data(), fileSize);

			return buffer;
		}

		/// <summary>
		/// Escribe los bytes en el archivo binario.
		/// </summary>
		void WriteBytes(const std::vector<char>& bytes) {
			OSK_ASSERT(IsWriteOpen(), "No se puede escribir en un archivo que no ha sido abierto.");
			OSK_ASSERT(IsBinaryOpen(), "No se puede leer en un archivo que no ha sido abierto.");

			outputStream.write(bytes.data(), bytes.size());
		}

		/// <summary>
		/// True si está abierto de cualquier modo.
		/// </summary>
		bool IsOpen() const {
			return inputStream.is_open() || outputStream.is_open();
		}

		/// <summary>
		/// True si está abierto para lectura.
		/// </summary>
		bool IsReadOpen() const {
			return inputStream.is_open();
		}

		/// <summary>
		/// True si está abierto para escritura.
		/// </summary>
		bool IsWriteOpen() const {
			return outputStream.is_open();
		}

		/// <summary>
		/// True si está abierto un archivo binario.
		/// </summary>
		bool IsBinaryOpen() const {
			return flags & (int)FileOpenType::BINARY;
		}

		/// <summary>
		/// True si el archivo existe.
		/// </summary>
		/// <param name="path">Ruta del archivo a comprobar.</param>
		static bool FileExists(const std::string& path) {
			std::ifstream stream(path);

			bool output = stream.good();

			if (stream.is_open())
				stream.close();

			return output;
		}

	private:

		/// <summary>
		/// FileOpenType.
		/// </summary>
		int flags = 0;

		std::ifstream inputStream;
		std::ofstream outputStream;

	};

}