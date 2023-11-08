#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::ECS {
	class ModelComponent3D;
}

namespace OSK::ASSETS {

	class Model3D;

	class OSKAPI_CALL ModelLoader3D final : public IAssetLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	private:

		UIndex64 m_nextModelId = 0;

	};

}
