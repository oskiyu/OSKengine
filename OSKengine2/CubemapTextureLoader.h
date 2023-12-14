#pragma once

#include "IAssetLoader.h"

#include "CubemapTexture.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL CubemapTextureLoader : public IAssetLoader, public TAssetLoader<CubemapTexture> {

	public:

		OSK_ASSET_TYPE_REG("OSK::CubemapTexture");

		OSK_DEFAULT_LOADER_IMPL(CubemapTexture);
		void Load(const std::string& assetFilePath, CubemapTexture* asset) override;

	};

}
