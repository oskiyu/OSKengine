#pragma once

#include "IAssetLoader.h"
#include "Font.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL FontLoader : public IAssetLoader, public TAssetLoader<Font> {

	public:

		OSK_ASSET_TYPE_REG("OSK::Font");

		OSK_DEFAULT_LOADER_IMPL(Font);
		void Load(const std::string& assetFilePath, Font* asset) override;

	};

}
