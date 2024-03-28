#pragma once

#include "IRenderSystem.h"

#include "Lights.h"
#include "ITopLevelAccelerationStructure.h"
#include "Model3D.h"
#include "Texture.h"
#include "GBuffer.h"
#include "RtRenderTarget.h"
#include "AssetRef.h"

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

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		void AddBlas(GRAPHICS::IBottomLevelAccelerationStructure* blas);

		const GRAPHICS::GpuBuffer* GetCameraBuffer(UIndex32 index) const {
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

		std::unordered_map<GameObjectIndex, glm::mat4> previousModelMatrices;


		// Renders
		void RenderGBuffer(GRAPHICS::ICommandList* cmdList);
		void RenderShadows(GRAPHICS::ICommandList* cmdList);
		void Resolve(GRAPHICS::ICommandList* cmdList);

		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::GBuffer gBuffer;
		GRAPHICS::GBuffer historicalGBuffer;

		GRAPHICS::DirectionalLight dirLight{};

		// Noises
		ASSETS::AssetRef<ASSETS::Texture> noise;
		ASSETS::AssetRef<ASSETS::Texture> noiseX;

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
		UniquePtr<GRAPHICS::GpuBuffer> cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<GRAPHICS::GpuBuffer> previousCameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<GRAPHICS::GpuBuffer> dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};

		// Materiales
		GRAPHICS::Material* gbufferMaterial = nullptr;
		GRAPHICS::Material* animatedGbufferMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> globalGbufferMaterialInstance;

	};

}
