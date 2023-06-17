#pragma once

#include "RenderSystem3D.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "GBuffer.h"
#include "RtRenderTarget.h"
#include "TaaProvider.h"

namespace OSK::ASSETS {
	class IrradianceMap;
	class SpecularMap;
}

namespace OSK::ECS {

	class CameraComponent3D;


	/// @brief 
	class OSKAPI_CALL PbrDeferredRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::PbrDeferredRenderSystem");

		PbrDeferredRenderSystem();

		void Initialize(
			ECS::GameObjectIndex cameraObject, 
			const ASSETS::IrradianceMap& irradianceMap, 
			const ASSETS::SpecularMap& specularMap);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnTick(TDeltaTime deltaTime) override;

		void ToggleTaa();

	private:

		/// @brief Estructura con la información almacenada
		/// en los uniform buffers de la cámara.
		struct CameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;

			alignas(16) Vector4f position;

			alignas(16) Vector2f nearFarPlanes;
		};

		/// @brief Estructura con la información almacenada
		/// en los uniform buffers de la cámara del frame
		/// anterior.
		struct PreviousCameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;
		};

	private:

		void LoadMaterials();
		void CreateBuffers();

		void SetupGBufferMaterial();
		void SetupResolveMaterial();

		void UpdateResolveMaterial();

		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderGBuffer(GRAPHICS::ICommandList* commandList);
		void ResolveGBuffer(GRAPHICS::ICommandList* commandList);
		void ExecuteTaa(GRAPHICS::ICommandList* commandList);
		void SharpenTaa(GRAPHICS::ICommandList* commandList);
		void CopyFinalImages(GRAPHICS::ICommandList* commandList);

		void GBufferRenderLoop(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType, UIndex32 jitterIndex);
		void ShadowsRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList, UIndex32 cascadeIndex);

	private:

		GRAPHICS::ComputeRenderTarget resolveRenderTarget;
		GRAPHICS::GBuffer gBuffer;

		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> cameraUbos{};
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> previousCameraUbos{};

		glm::mat4 previousCameraProjection = glm::mat4(1.0f);
		glm::mat4 previousCameraView = glm::mat4(1.0f);

		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> dirLightUbos{};
		GRAPHICS::DirectionalLight dirLight{};

		GRAPHICS::ShadowMap shadowMap;
		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* gbufferMaterial = nullptr;
		GRAPHICS::Material* animatedGbufferMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> globalGbufferMaterialInstance;

		HashMap<GameObjectIndex, glm::mat4> previousModelMatrices;

		GRAPHICS::Material* resolveMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> resolveMaterialInstance;

		GRAPHICS::TaaProvider taaProvider{};

		bool hasBeenInitialized = false;

	};

}
