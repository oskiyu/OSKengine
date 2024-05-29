#pragma once

#include "RenderSystem3D.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "GBuffer.h"
#include "RtRenderTarget.h"
#include "TaaProvider.h"

#include "OwnedPtr.h"

#include "IShaderPass.h"
#include "IDeferredResolver.h"

#include "MaterialInstance.h"
#include "PbrIblConfig.h"

#include "AssetRef.h"
#include "IrradianceMap.h"
#include "SpecularMap.h"

#include "Serializer.h"
#include "SavedGameObjectTranslator.h"
#include "ResourcesInFlight.h"


namespace OSK::ECS {

	class CameraComponent3D;


	/// @brief Sistema de renderizado en diferido.
	class OSKAPI_CALL DeferredRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::DeferredRenderSystem");
		OSK_SERIALIZABLE();

		DeferredRenderSystem();


		/// @brief Inicializa el sistema para poder renderizar.
		/// @param cameraObject Objeto de la c�mara.
		/// @param irradianceMap Mapa de irradiancia.
		/// @param specularMap Mapa especular.
		/// 
		/// @pre @p irradianceMap Debe haber sido cargado.
		/// @pre @p specularMap Debe haber sido cargado.
		/// 
		/// @pre @p cameraObject debe apuntar a un objeto con 
		/// los siguientes componentes:
		///		- CameraComponent3D
		///		- Transfomr3D
		void Initialize(
			ECS::GameObjectIndex cameraObject, 
			ASSETS::AssetRef<ASSETS::IrradianceMap> irradianceMap,
			ASSETS::AssetRef<ASSETS::SpecularMap> specularMap);

		/// @brief Actualiza la iluminaci�n IBL.
		/// @param irradianceMap Mapa de irradianca.
		/// @param specularMap Mapa especular.
		void SetIbl(
			ASSETS::AssetRef<ASSETS::IrradianceMap> irradianceMap,
			ASSETS::AssetRef<ASSETS::SpecularMap> specularMap);

		virtual void CreateTargetImage(const Vector2ui& size) override;
		virtual void Resize(const Vector2ui& size) override;


		/// @return GBuffer del renderizador.
		const GRAPHICS::GBuffer& GetGbuffer() const;
		
		/// @return GBuffer del renderizador.
		GRAPHICS::GBuffer& GetGbuffer();


		virtual void Execute(TDeltaTime deltaTime, std::span<const ECS::GameObjectIndex> objects) override;
		virtual void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;


		/// @brief Cambia el estado de TAA (on <-> off).
		void ToggleTaa();


		void AddShaderPass(OwnedPtr<GRAPHICS::IShaderPass> pass) override;
		void AddShadowsPass(OwnedPtr<GRAPHICS::IShaderPass> pass) override;

		/// @brief Establece el pase de resoluci�n.
		/// @param resolver Pase de resoluci�n.
		void SetResolver(OwnedPtr<GRAPHICS::IDeferredResolver> resolver);


		GRAPHICS::DirectionalLight& GetDirectionalLight();
		const GRAPHICS::DirectionalLight& GetDirectionalLight() const;

		GRAPHICS::PbrIblConfig& GetIblConfig();
		const GRAPHICS::PbrIblConfig& GetIblConfig() const;

		GRAPHICS::ShadowMap& GetShadowMap();
		const GRAPHICS::ShadowMap& GetShadowMap() const;


		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	protected:

		/// @brief Estructura con la informaci�n almacenada
		/// en los uniform buffers de la c�mara.
		struct CameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;

			alignas(16) Vector4f position;

			alignas(16) Vector2f nearFarPlanes;
		};

		/// @brief Estructura con la informaci�n almacenada
		/// en los uniform buffers de la c�mara del frame
		/// anterior.
		struct PreviousCameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;
		};

	protected:

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

	protected:

		// -- RENDER TARGETS & RESOLVE -- //

		/// @brief Render target donde se renderizar� la imagen final.
		GRAPHICS::ComputeRenderTarget m_resolveRenderTarget;

		/// @brief Geometry buffer.
		GRAPHICS::GBuffer m_gBuffer;


		/// @brief Contiene el slot de la c�mara.
		UniquePtr<GRAPHICS::MaterialInstance> m_gBufferCameraInstance = nullptr;

		/// @brief Pase de resoluci�n.
		UniquePtr<GRAPHICS::IDeferredResolver> m_resolverPass = nullptr;

		
		// -- C�MARA -- //

		/// @brief Objeto que contiene la c�mara desde la que se renderiza la escena.
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		/// @brief Proyecci�n de la c�mara en el fotograma anterior.
		glm::mat4 m_previousCameraProjection = glm::mat4(1.0f);

		/// @brief Fotograma de la c�mara en el fotograma actual.
		glm::mat4 m_previousCameraView = glm::mat4(1.0f);

		/// @brief Buffers CPU->GPU donde se almacena el estado de la c�mara en un fotograma.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_cameraBuffers {};

		/// @brief Buffers CPU->GPU donde se almacena el estado de la c�mara en el fotograma anterior.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_previousFrameCameraBuffers{};


		// -- LUCES Y SOMBRAS -- //

		/// @brief Informaci�n de la luz direccional.
		GRAPHICS::DirectionalLight m_directionalLight{};

		/// @brief Configuraci�n de la iluminaci�n IBL.
		GRAPHICS::PbrIblConfig m_iblConfig{};


		/// @brief Buffers CPU->GPU donde se almacena el estado de la luz direccional.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_directionalLightBuffers{};

		/// @brief Buffers CPU->GPU donde se almacena la configuraci�n de la iluminaci�n IBL.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_iblConfigBuffers{};

		ASSETS::AssetRef<ASSETS::IrradianceMap> m_irradianceMap;
		ASSETS::AssetRef<ASSETS::SpecularMap> m_specularMap;

		/// @brief Mapa de sombras en cascada.
		GRAPHICS::ShadowMap m_shadowMap;


		// -- TAA -- //

		/// @brief Implementaci�n del Temporal Anti-Aliasing.
		GRAPHICS::TaaProvider m_taaProvider{};

	};

}

//OSK_SYSTEM_SERIALIZATION(OSK::ECS::DeferredRenderSystem);
