#pragma once

#include "IAssetLoader.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL FontLoader : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::Font");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	};

}
