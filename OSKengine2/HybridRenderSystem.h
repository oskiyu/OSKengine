#pragma once

#include "IRenderSystem.h"

#include "Lights.h"
#include "ITopLevelAccelerationStructure.h"
#include "Model3D.h"
#include "Texture.h"

namespace OSK::GRAPHICS {
	class IBottomLevelAccelerationStructure;
}

namespace OSK::ASSETS {
	class IrradianceMap;
}

namespace OSK::ECS {

	class OSKAPI_CALL HybridRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::HybridRenderSystem");

		HybridRenderSystem();

		void Initialize(ECS::GameObjectIndex cameraObject, const ASSETS::IrradianceMap& irradianceMap);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnTick(TDeltaTime deltaTime) override;

		const static TSize GBUFFER_POSITION_TARGET_INDEX = 0;
		const static TSize GBUFFER_COLOR_TARGET_INDEX = 1;
		const static TSize GBUFFER_NORMAL_TARGET_INDEX = 2;
		const static TSize GBUFFER_VELOCITY_TARGET_INDEX = 3;

		void AddBlas(GRAPHICS::IBottomLevelAccelerationStructure* blas);

		const GRAPHICS::IGpuUniformBuffer* GetCameraBuffer(TIndex index) const {
			return cameraUbos[index].GetPointer();
		}

		const GRAPHICS::RenderTarget& GetShadowsIntermediateImage() const {
			return shadowsBuffer;
		}

		const GRAPHICS::RenderTarget& GetShadowsImage() const {
			return shadowsFinalMask;
		}

	private:

		// Setups
		void SetupRtResources();
		void SetupGBufferResources();

		void SetupShadowsInstance();
		void SetupGBufferInstance();
		void SetupResolveInstance();

		// Renders
		void RenderGBuffer(GRAPHICS::ICommandList* cmdList);
		void GBufferRenderLoop(GRAPHICS::ICommandList* cmdList, ASSETS::ModelType modelType);
		void RenderShadows(GRAPHICS::ICommandList* cmdList);
		void Resolve(GRAPHICS::ICommandList* cmdList);

		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::RenderTarget gBuffer;

		GRAPHICS::DirectionalLight dirLight{};

		// Noise
		ASSETS::Texture* noise = nullptr;

		// RayTracing
		UniquePtr<GRAPHICS::ITopLevelAccelerationStructure> topLevelAccelerationStructures[3]{};

		// Shadows
		GRAPHICS::Material* shadowsMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> shadowsMaterialInstance;

		GRAPHICS::Material* shadowsDenoiseMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> shadowsDenoiseMaterialInstance;

		GRAPHICS::RenderTarget shadowsBuffer;
		GRAPHICS::RenderTarget shadowsFinalMask; // final

		// Resolve
		GRAPHICS::Material* resolveMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> resolveMaterialInstance;

		// Resources
		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[3]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[3]{};

		// Materiales
		GRAPHICS::Material* gbufferMaterial = nullptr;
		GRAPHICS::Material* animatedGbufferMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> globalGbufferMaterialInstance;

	};

}
