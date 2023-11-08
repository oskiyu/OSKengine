#pragma once

#include "RenderSystem3D.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "GBuffer.h"
#include "RtRenderTarget.h"
#include "TaaProvider.h"

#include "OwnedPtr.h"

#include "IRenderPass.h"
#include "IDeferredResolver.h"

#include "MaterialInstance.h"

namespace OSK::ASSETS {
	class IrradianceMap;
	class SpecularMap;
}

namespace OSK::ECS {

	class CameraComponent3D;


	/// @brief 
	class OSKAPI_CALL DeferredRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::DeferredRenderSystem");

		DeferredRenderSystem();
		~DeferredRenderSystem();

		void Initialize(
			ECS::GameObjectIndex cameraObject, 
			const ASSETS::IrradianceMap& irradianceMap, 
			const ASSETS::SpecularMap& specularMap);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		const GRAPHICS::GBuffer& GetGbuffer() const;
		GRAPHICS::GBuffer& GetGbuffer();

		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnTick(TDeltaTime deltaTime) override;

		void ToggleTaa();

		void AddRenderPass(OwnedPtr<GRAPHICS::IRenderPass> pass) override;
		void SetResolver(OwnedPtr<GRAPHICS::IDeferredResolver> resolver);

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

		void ShadowsRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList, UIndex32 cascadeIndex);

	private:

		// -- RENDER TARGETS & RESOLVE -- //

		/// @brief Render target donde se renderizará la imagen final.
		GRAPHICS::ComputeRenderTarget m_resolveRenderTarget;

		/// @brief Geometry buffer.
		GRAPHICS::GBuffer m_gBuffer;


		/// @brief Contiene el slot de la cámara.
		UniquePtr<GRAPHICS::MaterialInstance> m_gBufferCameraInstance = nullptr;

		/// @brief Pase de resolución.
		UniquePtr<GRAPHICS::IDeferredResolver> m_resolverPass = nullptr;

		
		// -- CÁMARA -- //

		/// @brief Objeto que contiene la cámara desde la que se renderiza la escena.
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		/// @brief Proyección de la cámara en el fotograma anterior.
		glm::mat4 m_previousCameraProjection = glm::mat4(1.0f);

		/// @brief Fotograma de la cámara en el fotograma actual.
		glm::mat4 m_previousCameraView = glm::mat4(1.0f);

		/// @brief Buffers CPU->GPU donde se almacena el estado de la cámara en un fotograma.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraBuffers {};

		/// @brief Buffers CPU->GPU donde se almacena el estado de la cámara en el fotograma anterior.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_previousFrameCameraBuffers{};


		// -- LUCES Y SOMBRAS -- //

		/// @brief Información de la luz direccional.
		GRAPHICS::DirectionalLight m_directionalLight{};

		/// @brief Buffers CPU->GPU donde se almacena el estado de la luz direccional.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_directionalLightBuffers{};


		/// @brief Mapa de sombras en cascada.
		GRAPHICS::ShadowMap m_shadowMap;


		// -- TAA -- //

		/// @brief Implementación del Temporal Anti-Aliasing.
		GRAPHICS::TaaProvider m_taaProvider{};

	};

}
