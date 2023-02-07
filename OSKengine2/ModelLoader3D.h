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


		/// @brief Configura los material instances del modelo para que sean
		/// compatibles con los sistemas de renderizado 3D por defecto
		/// del motor.
		/// @param model Modelo original.
		/// @param component Componente del modelo 3D a configurar.
		/// 
		/// @pre component no debe ser null.
		static void SetupPbrModel(const Model3D& model, ECS::ModelComponent3D* component);

	};

}
