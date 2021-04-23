#pragma once
#include <fstream>
#include <vector>
#include <sstream>

	//Clase que actúa de interfaz para la lectura y escritura de atchivos.
class FileIO {

public:

	//Escribe un archivo de texto.
	//	<path>: ruta del archivo.
	//	<text>: texto que se va a escribir.
	static void WriteFile(const std::string& path, const std::string& text) {
		std::ofstream stream(path);
		if (stream.is_open()) {
			stream << text << std::endl;
		}

		stream.close();
	}

	//Lee un archivo de texto.
	//	<path>: ruta del archivo.
	static std::string ReadFromFile(const std::string& path) {
		std::ifstream stream(path);
		std::string line;
		std::string ret = "";
		if (stream.is_open()) {
			while (std::getline(stream, line)) {
				ret.append(line);
				ret.append("\n");
			}
		}

		stream.close();

		return ret;
	}

	//Lee un archivo binario.
	//	<filename>: ruta del archivo.
	static std::vector<char> ReadBinaryFromFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary); //Abre el archivo; ate -> al final del archivo

		//Error handling
		if (!file.is_open()) {
			throw std::runtime_error("ERROR: abrir archivo " + filename);
		}

		//Tamaño del std::vector
		size_t fileSize = (size_t)file.tellg();

		//Inicializar el std::vector
		std::vector<char> buffer(fileSize);

		//Leer el archivo desde el principio
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	//Comprueba si un archivo existe.
	//	<path>: ruta del archivo.
	static bool FileExists(const std::string& path) {
		std::ifstream stream(path);

		return stream.good();
	}

};
