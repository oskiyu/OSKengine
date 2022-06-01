#pragma once

#include "IAssetLoader.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL ModelLoader3D : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	};

}
