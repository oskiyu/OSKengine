#include "FontLoader.h"

#include "OSKengine.h"

#include "Font.h"
#include "FileIO.h"

#include "InvalidDescriptionFileException.h"

using namespace OSK;
using namespace OSK::ASSETS;

#include <json.hpp>

void FontLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	Font* output = (Font*)*asset;

	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string rawPath = assetInfo["raw_asset_path"];

	output->_SetFontFilePath(rawPath);
}
