#pragma once

#include "IAssetLoader.h"
#include "IGltfLoader.h"

#include "Collider.h"
#include "PreBuiltCollider.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL PreBuiltColliderLoader : public IAssetLoader, public TAssetLoader<PreBuiltCollider>, private IGltfLoader {

	public:

		OSK_ASSET_TYPE_REG("OSK::PreBuiltCollider");

		OSK_DEFAULT_LOADER_IMPL(PreBuiltCollider);
		void Load(const std::string& assetFilePath, PreBuiltCollider* asset) override;

	private:

		void ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) override final;

		COLLISION::Collider* m_buildingCollider = nullptr;

	};

}
