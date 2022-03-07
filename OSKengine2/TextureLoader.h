#pragma once

#include "IAssetLoader.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL TextureLoader : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::Texture");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	};

}
