#pragma once

#include "RenderSystem3D.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "GBuffer.h"

namespace OSK::ASSETS {
	class IrradianceMap;
}

namespace OSK::ECS {

	class CameraComponent3D;

	/// <summary>
	/// Sistema de renderizado PBR en diferido.
	/// </summary>
	class OSKAPI_CALL PbrDeferredRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::PbrDeferredRenderSystem");

		PbrDeferredRenderSystem();

		void Initialize(ECS::GameObjectIndex cameraObject, const ASSETS::IrradianceMap& irradianceMap);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnTick(TDeltaTime deltaTime) override;

		const static TSize GBUFFER_POSITION_TARGET_INDEX = 0;
		const static TSize GBUFFER_COLOR_TARGET_INDEX = 1;
		const static TSize GBUFFER_NORMAL_TARGET_INDEX = 2;

	private:

		void UpdateResolveMaterial();

		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderGBuffer(GRAPHICS::ICommandList* commandList);
		void Resolve(GRAPHICS::ICommandList* commandList);

		void GBufferRenderLoop(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType);
		void ShadowsRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList, TSize cascadeIndex);

		GRAPHICS::GBuffer gBuffer;

		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[3]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[3]{};
		GRAPHICS::DirectionalLight dirLight{};

		GRAPHICS::ShadowMap shadowMap;
		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* gbufferMaterial = nullptr;
		GRAPHICS::Material* animatedGbufferMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> globalGbufferMaterialInstance;

		GRAPHICS::Material* resolveMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> resolveMaterialInstance;

		bool hasBeenInitialized = false;

	};

}
