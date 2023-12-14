#include "HubCameraSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>

#include "CameraArmComponent.h"

#include <OSKengine/Logger.h>

void HubCameraSystem::OnCreate() {
	const auto* ecs = OSK::Engine::GetEcs();

	OSK::ECS::Signature signature{};
	signature.SetTrue(ecs->GetComponentType<CameraArmComponent>());
	signature.SetTrue(ecs->GetComponentType<OSK::ECS::Transform3D>());
	_SetSignature(signature);
}

void HubCameraSystem::OnTick(TDeltaTime deltaTime) {
	auto* ecs = OSK::Engine::GetEcs();

	for (const auto obj : GetObjects()) {

		const OSK::Vector2f cameraRotation = {
			0.15f,
			glm::sin(OSK::Engine::GetCurrentTime() * 0.5f) * 0.07f
		};

		auto& cameraArmTransform = ecs->GetComponent<OSK::ECS::Transform3D>(obj);

		cameraArmTransform.RotateWorldSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
		cameraArmTransform.RotateLocalSpace(glm::radians(cameraRotation.y * 0.25f), { 1, 0, 0 });

	}
}

void HubCameraSystem::OnObjectAdded(OSK::ECS::GameObjectIndex obj) {
	auto* ecs = OSK::Engine::GetEcs();

	const OSK::Vector2f cameraRotation = {
		0.0f,
		20.0f
	};

	auto& cameraArmTransform = ecs->GetComponent<OSK::ECS::Transform3D>(obj);

	cameraArmTransform.RotateWorldSpace(glm::radians(-cameraRotation.x), { 0, 1, 0 });
	cameraArmTransform.RotateLocalSpace(glm::radians(cameraRotation.y), { 1, 0, 0 });
}
