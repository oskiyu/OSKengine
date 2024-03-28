#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Model3D.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL ModelLoader3D final : public IAssetLoader, public TAssetLoader<Model3D> {

	public:

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		OSK_DEFAULT_LOADER_IMPL(Model3D);
		void Load(const std::string& assetFilePath, Model3D* asset) override;

	};

}
