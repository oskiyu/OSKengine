#pragma once

#include "IRenderSystem.h"

#include "MaterialInstance.h"
#include "GpuBuffer.h"

namespace OSK::ASSETS {
	class CubemapTexture;
	class Model3D;
}

namespace OSK::ECS {

	class OSKAPI_CALL SkyboxRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::SkyboxRenderSystem");

		SkyboxRenderSystem();

		void CreateTargetImage(const Vector2ui& size) override;

		void SetCamera(GameObjectIndex cameraObject);
		void SetCubemap(const ASSETS::CubemapTexture& texture);

		void Render(GRAPHICS::ICommandList* commandList) override;

	private:

		GRAPHICS::Material* skyboxMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> skyboxMaterialInstance;

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;

		UniquePtr<GRAPHICS::GpuBuffer> cameraUbos[3]{};

		ASSETS::Model3D* cubemapModel = nullptr;

	};

}
