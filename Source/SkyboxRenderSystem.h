#pragma once

#include "IRenderSystem.h"

#include "MaterialInstance.h"
#include "GpuBuffer.h"
#include "AssetRef.h"

#include "CubemapTexture.h"
#include "Model3D.h"

namespace OSK::ECS {

	class OSKAPI_CALL SkyboxRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::SkyboxRenderSystem");

		SkyboxRenderSystem();

		void CreateTargetImage(const Vector2ui& size) override;

		void SetCamera(GameObjectIndex cameraObject);
		void SetCubemap(ASSETS::AssetRef<ASSETS::CubemapTexture> texture);

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

	private:

		GRAPHICS::Material* skyboxMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> skyboxMaterialInstance;

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;

		UniquePtr<GRAPHICS::GpuBuffer> cameraUbos[3]{};

		ASSETS::AssetRef<ASSETS::CubemapTexture> m_skybox;
		ASSETS::AssetRef<ASSETS::Model3D> m_cubemapModel;

	};

}
