// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "IRenderSystem.h"

#include "IGpuUniformBuffer.h"

namespace OSK::ASSETS {
	class Model3D;
}
namespace OSK::COLLISION {
	class IBottomLevelCollider;
}

namespace OSK::ECS {

	class OSKAPI_CALL ColliderRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::ColliderRenderSystem");

		ColliderRenderSystem();

		void Initialize(GameObjectIndex cameraObject);

		void CreateTargetImage(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;

	private:

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;
		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};

		GRAPHICS::Material* material = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> materialInstance;

		const ASSETS::Model3D* cubeModel = nullptr;
		const ASSETS::Model3D* sphereModel = nullptr;

	};

}
