#pragma once

#include "IAssetLoader.h"

#include "AudioAsset.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL AudioLoader : public IAssetLoader, public TAssetLoader<AudioAsset> {

	public:

		OSK_ASSET_TYPE_REG("OSK::AudioAsset");

		OSK_DEFAULT_LOADER_IMPL(AudioAsset);
		void Load(const std::string& assetFilePath, AudioAsset* asset) override;

	};

}
