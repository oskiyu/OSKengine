#pragma once

#include "IAssetLoader.h"

namespace OSK::ECS {
	class ModelComponent3D;
}

namespace OSK::ASSETS {

	class Model3D;

	class OSKAPI_CALL ModelLoader3D : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

		static void SetupPbrModel(Model3D* model, ECS::ModelComponent3D* component);

	};

}
