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

		/// <summary>
		/// Configura los material instances del modelo para que sean
		/// compatibles con los sistemas de renderizado 3D por defecto
		/// del motor.
		/// </summary>
		/// 
		/// @pre model no debe ser null.
		/// @pre component no debe ser null.
		static void SetupPbrModel(Model3D* model, ECS::ModelComponent3D* component);

	};

}
