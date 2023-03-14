#pragma once

#include "IRenderSystem.h"

#include "Lights.h"
#include "ITopLevelAccelerationStructure.h"
#include "Model3D.h"
#include "Texture.h"
#include "GBuffer.h"
#include "RtRenderTarget.h"

namespace OSK::GRAPHICS {
	class IBottomLevelAccelerationStructure;
}

namespace OSK::ASSETS {
	class IrradianceMap;
	class SpecularMap;
}

namespace OSK::ECS {

	class OSKAPI_CALL HybridRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::HybridRenderSystem");

		HybridRenderSystem();

		void Initialize(
			ECS::GameObjectIndex cameraObject, 
			const ASSETS::IrradianceMap& irradianceMap, 
			const ASSETS::SpecularMap& specularMap);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnTick(TDeltaTime deltaTime) override;

		void AddBlas(GRAPHICS::IBottomLevelAccelerationStructure* blas);

		const GRAPHICS::IGpuUniformBuffer* GetCameraBuffer(TIndex index) const {
			return cameraUbos[index].GetPointer();
		}

		const GRAPHICS::RtRenderTarget& GetShadowsIntermediateImage() const {
			return raytracedShadowsTarget;
		}

		const GRAPHICS::RtRenderTarget& GetShadowsImage() const {
			return finalShadowsTarget;
		}

	private:

		// Setups
		void SetupRtResources();
		void SetupGBufferResources();

		void SetupShadowsInstance();
		void SetupGBufferInstance();
		void SetupResolveInstance();


		// Velocity buffer
		glm::mat4 previousView = glm::mat4(1.0f);
		glm::mat4 previousProjection = glm::mat4(1.0f);

		HashMap<GameObjectIndex, glm::mat4> previousModelMatrices;


		// Renders
		void RenderGBuffer(GRAPHICS::ICommandList* cmdList);
		void GBufferRenderLoop(GRAPHICS::ICommandList* cmdList, ASSETS::ModelType modelType);
		void RenderShadows(GRAPHICS::ICommandList* cmdList);
		void Resolve(GRAPHICS::ICommandList* cmdList);

		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::GBuffer gBuffer;
		GRAPHICS::GBuffer historicalGBuffer;

		GRAPHICS::DirectionalLight dirLight{};

		// Noises
		ASSETS::Texture* noise = nullptr;
		ASSETS::Texture* noiseX = nullptr;

		// RayTracing
		UniquePtr<GRAPHICS::ITopLevelAccelerationStructure> topLevelAccelerationStructures[3]{};

		// Shadows
		GRAPHICS::Material* shadowsMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> shadowsMaterialInstance;

		GRAPHICS::Material* shadowsReprojectionMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> shadowsReprojectionMaterialInstance;

		GRAPHICS::Material* shadowsAtrousMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> shadowsAtrousMaterialInstance;

		GRAPHICS::RtRenderTarget raytracedShadowsTarget;
		GRAPHICS::ComputeRenderTarget reprojectedShadowsTarget;
		GRAPHICS::ComputeRenderTarget finalShadowsTarget;

		GRAPHICS::ComputeRenderTarget historicalShadowsTarget;

		// Resolve
		GRAPHICS::Material* resolveMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> resolveMaterialInstance;

		// Resources
		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> previousCameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};

		// Materiales
		GRAPHICS::Material* gbufferMaterial = nullptr;
		GRAPHICS::Material* animatedGbufferMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> globalGbufferMaterialInstance;

	};

}
