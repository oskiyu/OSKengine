#include "FontLoader.h"

#include "OSKengine.h"

#include "Font.h"
#include "FileIO.h"

#include "InvalidDescriptionFileException.h"

using namespace OSK;
using namespace OSK::ASSETS;

#include <json.hpp>

AssetOwningRef<Font> FontLoader::Load(const std::string& assetFilePath) {
	AssetOwningRef<Font> output(assetFilePath);

	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string rawPath = assetInfo["raw_asset_path"];

	output->_SetFontFilePath(rawPath);

	return output;
}
