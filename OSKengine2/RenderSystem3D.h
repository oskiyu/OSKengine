#pragma once

#include "IRenderSystem.h"

#include "ShadowMap.h"
#include "Lights.h"
#include "IGpuUniformBuffer.h"
#include "TerrainComponent.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ASSETS {
	class IrradianceMap;
	class Texture;
	enum class ModelType;
}

namespace OSK::ECS {

	class CameraComponent3D;

	/// <summary>
	/// Sistema que se encarga del renderizado de modelos 3D de los objetos.
	/// 
	/// Signature:
	/// - ModelComponent3D.
	/// - Transform3D.
	/// </summary>
	class OSKAPI_CALL RenderSystem3D : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem3D");

		RenderSystem3D();

		void Initialize(ECS::GameObjectIndex cameraObject, const ASSETS::IrradianceMap& irradianceMap);

		void InitializeTerrain(const Vector2ui& resolution, const ASSETS::Texture& heightMap, const ASSETS::Texture& texture);

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void OnTick(TDeltaTime deltaTime) override;
		void Render(GRAPHICS::ICommandList* commandList) override;
		
		GRAPHICS::ShadowMap* GetShadowMap();
		
		const GRAPHICS::IGpuUniformBuffer* GetCameraBuffer(TIndex index) const {
			return cameraUbos[index].GetPointer();
		}

	private:
		
		void GenerateShadows(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType);
		void RenderScene(GRAPHICS::ICommandList* commandList);

		void SceneRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList);
		void ShadowsRenderLoop(ASSETS::ModelType modelType, GRAPHICS::ICommandList* commandList, TSize cascadeIndex);

		void RenderTerrain(GRAPHICS::ICommandList* commandList);

		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[3]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[3]{};
		GRAPHICS::DirectionalLight dirLight{};

		GRAPHICS::ShadowMap shadowMap;
		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* sceneMaterial = nullptr;
		GRAPHICS::Material* animatedSceneMaterial = nullptr;

		UniquePtr<GRAPHICS::MaterialInstance> sceneMaterialInstance;

		TerrainComponent terrain{};
		GRAPHICS::Material* terrainMaterial = nullptr;

	};

}
