#pragma once

#include "IAssetLoader.h"
#include "IGltfLoader.h"
#include "UniquePtr.hpp"

#include "PreBuiltSpline3D.h"


namespace OSK::ASSETS {
	
	class PreBuiltSplineLoader3D : public IAssetLoader, public TAssetLoader<PreBuiltSpline3D>, private IGltfLoader {

	public:

		PreBuiltSplineLoader3D() = default;

		OSK_ASSET_TYPE_REG("OSK::PreBuiltSpline3D");

		OSK_DEFAULT_LOADER_IMPL(PreBuiltSpline3D);
		void Load(const std::string& assetFilePath, PreBuiltSpline3D* asset) override;

	private:

		void ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) override final;

		PreBuiltSpline3D* m_loaded = nullptr;

	};
	
}
