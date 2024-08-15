#include "FileIO.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Assert.h"
#include "FileNotFoundException.h"
#include "OSKengine.h"

using namespace OSK;
using namespace OSK::IO;

void FileIO::WriteFile(std::string_view path, const std::string& text) {
	std::ofstream stream(path.data());
	stream << text << std::endl;

	stream.close();
}

void FileIO::WriteBinaryFile(std::string_view path, std::span<TByte const> data) {
	std::ofstream stream(path.data(), std::ios::out | std::ios::binary);
	stream.write(reinterpret_cast<const char*>(data.data()), data.size());
}


std::string FileIO::ReadFromFile(std::string_view path) {
	OSK_ASSERT(FileExists(path), FileNotFoundException(path));

	std::ifstream stream(path.data());
	std::string line;
	std::string ret = "";

	while (std::getline(stream, line)) {
		ret.append(line);
		ret.append("\n");
	}

	stream.close();

	return ret;
}


DynamicArray<char> FileIO::ReadBinaryFromFile(std::string_view filename) {
	OSK_ASSERT(FileExists(filename), FileNotFoundException(filename));

	std::ifstream file(filename.data(), std::ios::ate | std::ios::binary); //Abre el archivo; ate -> al final del archivo

	//Tamaño del std::vector
	const auto fileSize = static_cast<USize64>(file.tellg());

	//Inicializar el std::vector
	DynamicArray<char> buffer = DynamicArray<char>::CreateResized(fileSize);

	//Leer el archivo desde el principio
	file.seekg(0);
	file.read(buffer.GetData(), fileSize);

	file.close();

	return buffer;
}

bool FileIO::FileExists(std::string_view path) {
	std::ifstream stream(path.data());

	bool output = stream.good();

	if (stream.is_open())
		stream.close();

	return output;
}
