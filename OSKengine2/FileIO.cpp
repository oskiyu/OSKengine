#include "FileIO.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Assert.h"

using namespace OSK;

void FileIO::WriteFile(const std::string& path, const std::string& text) {
	OSK_ASSERT(FileExists(path), std::string("Se ha intentado escribir el archivo ") + path + std::string(" pero no existe."));

	std::ofstream stream(path);
	stream << text << std::endl;

	stream.close();
}


std::string FileIO::ReadFromFile(const std::string& path) {
	OSK_ASSERT(FileExists(path), std::string("Se ha intentado leer el archivo ") + path + std::string(" pero no existe."));

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


DynamicArray<char> FileIO::ReadBinaryFromFile(const std::string& filename) {
	OSK_ASSERT(FileExists(filename), std::string("El archivo binario ") + filename + std::string(" no existe."));

	std::ifstream file(filename, std::ios::ate | std::ios::binary); //Abre el archivo; ate -> al final del archivo

	//Tama�o del std::vector
	size_t fileSize = (size_t)file.tellg();

	//Inicializar el std::vector
	DynamicArray<char> buffer = DynamicArray<char>::CreateResizedArray(fileSize);

	//Leer el archivo desde el principio
	file.seekg(0);
	file.read(buffer.GetData(), fileSize);

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