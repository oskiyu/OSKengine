#pragma once

#include "IAssetLoader.h"
#include "IGltfLoader.h"

#include "Collider.h"
#include "PreBuiltCollider.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL PreBuiltColliderLoader : public IAssetLoader, public TAssetLoader<PreBuiltCollider> {

	public:

		OSK_ASSET_TYPE_REG("OSK::PreBuiltCollider");

		OSK_DEFAULT_LOADER_IMPL(PreBuiltCollider);
		void Load(const std::string& assetFilePath, PreBuiltCollider* asset) override;

	};

}
