#pragma once

#include "CarAssets.h"
#include <OSKengine/IAssetLoader.h>

class CarAssetsLoader : public OSK::ASSETS::IAssetLoader, public OSK::ASSETS::TAssetLoader<CarAssets> {

public:

	OSK_ASSET_TYPE_REG("CarAssets");

	OSK_DEFAULT_LOADER_IMPL(CarAssets);
	void Load(const std::string& assetFilePath, CarAssets* asset) override;

};
