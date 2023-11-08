#pragma once

#include "IAssetLoader.h"
#include "IGltfLoader.h"

#include "Collider.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL PreBuiltColliderLoader : public IAssetLoader, private IGltfLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::PreBuiltCollider")

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	private:

		void ProcessNode(const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) override final;

		COLLISION::Collider* m_buildingCollider = nullptr;

	};

}
