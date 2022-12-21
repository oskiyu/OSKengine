// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "Material.h"
#include "MaterialInstance.h"
#include "RenderTarget.h"

#include <glm.hpp>

namespace OSK::GRAPHICS {
	class ICommandList;
	class GpuImage;
}

namespace OSK::ASSETS {

	class Model3D;

	class OSKAPI_CALL SpecularMapLoader : public IAssetLoader {

	public:

		SpecularMapLoader();
		~SpecularMapLoader();

		OSK_ASSET_TYPE_REG("OSK::SpecularMap");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	private:

		void UploadImage(GRAPHICS::GpuImage* img, const float* pixels, const Vector3ui& size) const;

		void DrawPreFilter(GRAPHICS::GpuImage* cubemap, GRAPHICS::ICommandList* cmdList, TIndex mipLevel, float roughness);

		ASSETS::Model3D* cubemapModel = nullptr;

		const Vector2ui maxResolution = { 1024u };

		GRAPHICS::RenderTarget cubemapRenderTarget;

		GRAPHICS::Material* prefilterMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> prefilterMaterialInstance;

		static glm::mat4 cameraProjection;
		static glm::mat4 cameraViews[6];


	};

}
