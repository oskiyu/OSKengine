#pragma once

#include "IAssetLoader.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL AudioLoader : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::AudioAsset")

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	};

}
