#include "FileIO.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Assert.h"
#include "FileNotFoundException.h"
#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::IO;

#ifdef OSK_ANDROID
#include <android/asset_manager.h>
#include <android/log.h>
AAssetManager* FileIO::m_androidAssetManager = nullptr;
#endif

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
#ifdef OSK_ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s %s", "Tratando de abrir el archivo ", path.data());
	if (auto* file = AAssetManager_open(m_androidAssetManager, path.data(), AASSET_MODE_BUFFER)) {
		const auto size = AAsset_getLength64(file);
		auto data = DynamicArray<char>::CreateResized(size);
		AAsset_read(file, data.GetData(), size);
		AAsset_close(file);

		std::string output{};
		output.assign((const char*)data.GetData(), size);

		__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s", "Archivo leído por el AssetManager de Android.");

		return output;
	}
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s", "Archivo NO se puede leer por el AssetManager de Android.");
#endif // OSK_ANDROID

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
#ifdef OSK_ANDROID
	if (auto* file = AAssetManager_open(m_androidAssetManager, filename.data(), AASSET_MODE_BUFFER)) {
		const auto size = AAsset_getLength64(file);
		auto data = DynamicArray<char>::CreateResized(size);
		AAsset_read(file, data.GetData(), size);
		AAsset_close(file);

		return data;
	}
#endif // OSK_ANDROID

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
#ifdef OSK_ANDROID
	if (auto* file = AAssetManager_open(m_androidAssetManager, path.data(), AASSET_MODE_BUFFER)) {
		AAsset_close(file);
		return true;
	}
#endif // OSK_ANDROID

	std::ifstream stream(path.data());

	bool output = stream.good();

	if (stream.is_open())
		stream.close();

	return output;
}

#ifdef OSK_ANDROID
void FileIO::SetAssetManager(AAssetManager* manager) {
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s %i", "Establecido el AssetManager", manager);
	m_androidAssetManager = manager;
}
#endif
