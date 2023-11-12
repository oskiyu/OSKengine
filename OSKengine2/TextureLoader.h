#pragma once

#include "IAssetLoader.h"
#include "Texture.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL TextureLoader : public IAssetLoader, public TAssetLoader<Texture> {

	public:

		OSK_ASSET_TYPE_REG("OSK::Texture");

		OSK_DEFAULT_LOADER_IMPL(Texture);
		AssetOwningRef<Texture> Load(const std::string& assetFilePath) override;

	};

}
