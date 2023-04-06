#pragma once

#include "IRenderSystem.h"

#include "ShadowMap.h"
#include "Lights.h"
#include "IGpuUniformBuffer.h"
#include "TerrainComponent.h"
#include "RtRenderTarget.h"
#include "TaaProvider.h"

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

		void OnTick(TDeltaTime deltaTime) override;
		void Render(GRAPHICS::ICommandList* commandList) override;
		
		GRAPHICS::ShadowMap* GetShadowMap();
		
		const GRAPHICS::IGpuUniformBuffer* GetCameraBuffer(TIndex index) const {
			return cameraBuffers[index].GetPointer();
		}

		inline void ToggleTaa() { taaProvider.ToggleActivation(); }

		constexpr static TIndex COLOR_IMAGE_INDEX = 0;
		constexpr static TIndex MOTION_IMAGE_INDEX = 1;

	private:
		
		void CreateBuffers();
		void LoadMaterials();
		void SetupMaterials();

		void GenerateShadows(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType);
		void RenderScene(GRAPHICS::ICommandList* commandList);

		void ExecuteTaa(GRAPHICS::ICommandList* commandList);
		void CopyTaaResult(GRAPHICS::ICommandList* commandList);

		void SceneRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList);
		void ShadowsRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList, TSize cascadeIndex);

		void RenderTerrain(GRAPHICS::ICommandList* commandList);

		/// @brief Buffer que contendrá la resolución del sistema.
		UniquePtr<GRAPHICS::IGpuUniformBuffer> resolutionBuffer = nullptr;

		/// @brief Buffers con la información de la cámara en un frame en concreto.
		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraBuffers[NUM_RESOURCES_IN_FLIGHT]{};
		/// @brief Buffers con la información de la cámara en el frame anterior.
		UniquePtr<GRAPHICS::IGpuUniformBuffer> previousCameraBuffers[NUM_RESOURCES_IN_FLIGHT]{};

		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};
		GRAPHICS::DirectionalLight dirLight{};

		GRAPHICS::ShadowMap shadowMap;
		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* sceneMaterial = nullptr;
		GRAPHICS::Material* animatedSceneMaterial = nullptr;

		UniquePtr<GRAPHICS::MaterialInstance> sceneMaterialInstance;

		TerrainComponent terrain{};
		GRAPHICS::Material* terrainMaterial = nullptr;

		// TAA
		GRAPHICS::TaaProvider taaProvider{};

		HashMap<GameObjectIndex, glm::mat4> previousModelMatrices;

		glm::mat4 previousCameraProjection = glm::mat4(1.0f);
		glm::mat4 previousCameraView = glm::mat4(1.0f);

	};

}
