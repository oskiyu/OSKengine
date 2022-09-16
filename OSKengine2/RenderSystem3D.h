#pragma once

#include "IRenderSystem.h"

#include "ShadowMap.h"
#include "Lights.h"
#include "IGpuUniformBuffer.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ASSETS {
	class IrradianceMap;
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

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;
		
		GRAPHICS::ShadowMap* GetShadowMap();
		
	private:
		
		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderScene(GRAPHICS::ICommandList* commandList);

		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[3]{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUbos[3]{};
		GRAPHICS::DirectionalLight dirLight{};

		GRAPHICS::ShadowMap shadowMap;
		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		GRAPHICS::Material* sceneMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> sceneMaterialInstance;

	};

}
