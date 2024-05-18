#pragma once

#include "IRenderSystem.h"

#include "ShadowMap.h"
#include "Lights.h"
#include "GpuBuffer.h"
#include "TerrainComponent.h"
#include "RtRenderTarget.h"
#include "TaaProvider.h"

#include <array>
#include <span>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ASSETS {
	class IrradianceMap;
	class SpecularMap;
	class CubemapTexture;
	class Texture;
	enum class ModelType;
}

namespace OSK::ECS {

	class CameraComponent3D;

	/// @brief Sistema que se encarga del renderizado de modelos 3D de los objetos.
	/// 
	/// Signature:
	/// - ModelComponent3D.
	/// - Transform3D.
	class OSKAPI_CALL RenderSystem3D : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem3D");

		RenderSystem3D();

		void Initialize(GameObjectIndex cameraObject, const ASSETS::IrradianceMap& irradianceMap, const ASSETS::SpecularMap& specularMap);

		void InitializeTerrain(const Vector2ui& resolution, const ASSETS::Texture& heightMap, const ASSETS::Texture& texture);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Execute(TDeltaTime deltaTime, std::span<const ECS::GameObjectIndex> objects) override;
		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;
		
		GRAPHICS::ShadowMap* GetShadowMap();
		
		const GRAPHICS::GpuBuffer* GetCameraBuffer(UIndex32 index) const {
			return m_cameraBuffers[index].GetPointer();
		}

		inline void ToggleTaa() { m_taaProvider.ToggleActivation(); }

		constexpr static UIndex32 COLOR_IMAGE_INDEX  = 0;
		constexpr static UIndex32 MOTION_IMAGE_INDEX = 1;
		constexpr static UIndex32 NORMAL_IMAGE_INDEX = 2;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;

	private:
		
		void CreateBuffers();
		void LoadMaterials();
		void SetupMaterials();

		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderScene(GRAPHICS::ICommandList* commandList);

		void ExecuteTaa(GRAPHICS::ICommandList* commandList);
		void CopyTaaResult(GRAPHICS::ICommandList* commandList);

		void RenderTerrain(GRAPHICS::ICommandList* commandList);

		/// @brief Buffer que contendrá la resolución del sistema.
		UniquePtr<GRAPHICS::GpuBuffer> m_resolutionBuffer = nullptr;

		/// @brief Buffers con la información de la cámara en un frame en concreto.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraBuffers{};
		/// @brief Buffers con la información de la cámara en el frame anterior.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_previousCameraBuffers{};

		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_dirLightUbos{};
		GRAPHICS::DirectionalLight m_dirLight{};

		GRAPHICS::ShadowMap m_shadowMap;
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* m_sceneMaterial = nullptr;
		GRAPHICS::Material* m_animatedSceneMaterial = nullptr;

		UniquePtr<GRAPHICS::MaterialInstance> m_sceneMaterialInstance;

		TerrainComponent m_terrain{};
		GRAPHICS::Material* m_terrainMaterial = nullptr;

		// TAA
		GRAPHICS::TaaProvider m_taaProvider{};

		std::unordered_map<GameObjectIndex, glm::mat4> m_previousModelMatrices;

		glm::mat4 m_previousCameraProjection = glm::mat4(1.0f);
		glm::mat4 m_previousCameraView = glm::mat4(1.0f);

	};

}
