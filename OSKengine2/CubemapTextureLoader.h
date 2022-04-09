#pragma once

#include "IAssetLoader.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL CubemapTextureLoader : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::CubemapTexture");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	};

}
