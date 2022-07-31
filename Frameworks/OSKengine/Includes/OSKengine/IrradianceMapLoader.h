#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "Material.h"
#include "MaterialInstance.h"
#include "RenderTarget.h"

#include <glm.hpp>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ASSETS {

	class Model3D;

	class OSKAPI_CALL IrradianceMapLoader : public IAssetLoader {

	public:

		IrradianceMapLoader();
		~IrradianceMapLoader();

		OSK_ASSET_TYPE_REG("OSK::IrradianceMap");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	private:

		void DrawCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList);

		void GenCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList);
		void ConvoluteCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList);

		GRAPHICS::Material* cubemapGenMaterial = nullptr;
		GRAPHICS::Material* cubemapConvolutionMaterial = nullptr;

		UniquePtr<GRAPHICS::MaterialInstance> cubemapGenMaterialInstance = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> cubemapConvolutionMaterialInstance = nullptr;

		GRAPHICS::RenderTarget cubemapGenRenderTarget;

		Model3D* cubemapModel = nullptr;

		const Vector2ui irradianceLayerSize = Vector2ui(512u, 512u);

		static glm::mat4 cameraProjection;
		static glm::mat4 cameraViews[6];

	};

}
