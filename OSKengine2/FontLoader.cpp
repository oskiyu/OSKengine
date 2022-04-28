#include "FontLoader.h"

#include "Font.h"
#include "FileIO.h"

using namespace OSK;
using namespace OSK::ASSETS;

#include <json.hpp>

void FontLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	Font* output = (Font*)*asset;

	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));

	OSK_ASSERT(assetInfo.contains("file_type"), "Archivo de textura incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(assetInfo.contains("spec_ver"), "Archivo de textura incorrecto: no se encuentra 'spec_ver'.");
	OSK_ASSERT(assetInfo.contains("name"), "Archivo de textura incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(assetInfo.contains("asset_type"), "Archivo de textura incorrecto: no se encuentra 'asset_type'.");
	OSK_ASSERT(assetInfo.contains("raw_asset_path"), "Archivo de textura incorrecto: no se encuentra 'raw_asset_path'.");

	std::string rawPath = assetInfo["raw_asset_path"];

	output->_SetFontFilePath(rawPath);
}
