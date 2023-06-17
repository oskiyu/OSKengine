#include "IAssetLoader.h"

#include "Assert.h"
#include "InvalidDescriptionFileException.h"
#include "RawAssetFileNotFoundException.h"
#include "AssetDescriptionFileNotFoundException.h"

#include "FileIO.h"

#include "OSKengine.h"

#include <json.hpp>

nlohmann::json OSK::ASSETS::IAssetLoader::ValidateDescriptionFile(std::string_view filePath) const {
	OSK_ASSERT(IO::FileIO::FileExists(filePath), AssetDescriptionFileNotFoundException(filePath));

	const nlohmann::json file = nlohmann::json::parse(IO::FileIO::ReadFromFile(filePath));

	OSK_ASSERT(file.contains("file_type"), InvalidDescriptionFileException("No se encuentra file_type", filePath));
	OSK_ASSERT(file.contains("spec_ver"), InvalidDescriptionFileException("No se encuentra spec_ver", filePath));
	OSK_ASSERT(file.contains("name"), InvalidDescriptionFileException("No se encuentra name", filePath));
	OSK_ASSERT(file.contains("asset_type"), InvalidDescriptionFileException("No se encuentra asset_type", filePath));
	OSK_ASSERT(file.contains("raw_asset_path"), InvalidDescriptionFileException("No se encuentra raw_asset_path", filePath));

	OSK_ASSERT(IO::FileIO::FileExists(file["raw_asset_path"]), RawAssetFileNotFoundException(file["raw_asset_path"]));

	return file;
}
