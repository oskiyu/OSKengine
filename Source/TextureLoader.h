#pragma once

#include "IAssetLoader.h"
#include "Texture.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL TextureLoader : public IAssetLoader, public TAssetLoader<Texture> {

	public:

		OSK_ASSET_TYPE_REG("OSK::Texture");

		OSK_DEFAULT_LOADER_IMPL(Texture);
		void Load(const std::string& assetFilePath, Texture* asset) override;

		void RegisterTexture(AssetOwningRef<Texture>&& texture);

	private:

		DynamicArray<AssetOwningRef<Texture>> m_externallyLoadedTextures;

	};

}
