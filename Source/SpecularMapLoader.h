// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "Material.h"
#include "MaterialInstance.h"
#include "RtRenderTarget.h"
#include "RenderTarget.h"
#include "AssetRef.h"
#include "SpecularMap.h"

#include <glm/glm.hpp>

namespace OSK::GRAPHICS {
	class ICommandList;
	class GpuImage;
}

namespace OSK::ASSETS {

	class Model3D;

	class OSKAPI_CALL SpecularMapLoader : public IAssetLoader, public TAssetLoader<SpecularMap> {

	public:

		SpecularMapLoader();

		OSK_ASSET_TYPE_REG("OSK::SpecularMap");

		OSK_DEFAULT_LOADER_IMPL(SpecularMap);
		void Load(const std::string& assetFilePath, SpecularMap* asset) override;

	private:

		void UploadImage(GRAPHICS::GpuImage* img, const float* pixels, const Vector3ui& size) const;

		void DrawOriginal(GRAPHICS::GpuImage* cubemap, GRAPHICS::ICommandList* cmdList);
		void DrawPreFilter(GRAPHICS::GpuImage* cubemap, GRAPHICS::ICommandList* cmdList, UIndex32 mipLevel, float roughness);
		void GenerateLut(GRAPHICS::ICommandList* cmdList);

		ASSETS::AssetRef<ASSETS::Model3D> cubemapModel;

		// Resolución del nivel de detalle más alto del specular map.
		const Vector2ui maxResolution = Vector2ui(1024u);

		// Material para renderizar cubemap a partir de una imagen 2D
		// 2D -> cubemap
		GRAPHICS::Material* generationMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> generationMaterialInstance;

		GRAPHICS::RenderTarget cubemapRenderTarget;

		// Generación de los mapas especulares
		GRAPHICS::Material* prefilterMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> prefilterMaterialInstance;

		// Generación de LUT
		GRAPHICS::Material* lutGenerationMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> lutGenerationMaterialInstance;
		GRAPHICS::ComputeRenderTarget lookUpTable;

		static glm::mat4 cameraProjection;
		static glm::mat4 cameraViews[6];

	};

}
