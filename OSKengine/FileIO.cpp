#include "FileIO.h"

#include <iostream>
#include <fstream>

namespace OSK {

	void FileIO::WriteFile(const std::string& path, const std::string& text) {
		std::ofstream stream(path);
		stream << text << std::endl;
		
		stream.close();
	}


	std::string FileIO::ReadFromFile(const std::string& path) {
		OSK_ASSERT(FileExists(path), "Se ha intentado leer el archivo " + path + " pero no existe.");

		std::ifstream stream(path);
		std::string line;
		std::string ret = "";
		
		while (std::getline(stream, line)) {
			ret.append(line);
			ret.append("\n");
		}
		
		stream.close();
			
		return ret;
	}


	std::vector<char> FileIO::ReadBinaryFromFile(const std::string& filename) {
		OSK_ASSERT(FileExists(filename), "El archivo binarion " + filename + " no existe.");

		std::ifstream file(filename, std::ios::ate | std::ios::binary); //Abre el archivo; ate -> al final del archivo

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

	bool FileIO::FileExists(const std::string& path) {
		std::ifstream stream(path);

		bool output = stream.good();

		if (stream.is_open())
			stream.close();

		return output;
	}

}
