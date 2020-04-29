#include "FileIO.h"

#include <iostream>
#include <fstream>

namespace OSK {

	void FileIO::WriteFile(const std::string& path, const std::string& text) {
		std::ofstream stream(path);
		if (stream.is_open()) {
			stream << text << std::endl;
		}
		else
			Logger::Log(LogMessageLevels::BAD_ERROR, "WriteFile to " + path);

		stream.close();
	}


	std::string FileIO::ReadFromFile(const std::string& path) {
		std::ifstream stream(path);
		std::string line;
		std::string ret = "";
		if (stream.is_open()) {
			while (std::getline(stream, line)) {
				ret.append(line);
				ret.append(std::to_string('\n'));
			}
		}
		else
			Logger::Log(LogMessageLevels::BAD_ERROR, "ReadFromFile from " + path);
		
		stream.close();
			
		return ret;
	}


	std::vector<char> FileIO::ReadBinaryFromFile(const std::string& filename) {
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

}