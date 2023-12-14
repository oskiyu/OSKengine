#include "CameraAttachmentSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/IUserInput.h>
#include <OSKengine/IMouseInput.h>
#include <OSKengine/MouseState.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

void CameraAttachmentSystem::OnCreate() {
	OSK::ECS::Signature signature{};

	this->_SetSignature(signature);
}

void CameraAttachmentSystem::OnTick(TDeltaTime deltaTime) {
	const auto* ecs = OSK::Engine::GetEcs();
	
	if (m_cameraObject == OSK::ECS::EMPTY_GAME_OBJECT || m_carObject == OSK::ECS::EMPTY_GAME_OBJECT) {
		return;
	}

	const auto& carTransform = ecs->GetComponent<OSK::ECS::Transform3D>(m_carObject);
	auto& cameraTransform = ecs->GetComponent<OSK::ECS::Transform3D>(m_cameraObject);

	const auto& carPhysics = ecs->GetComponent<OSK::ECS::PhysicsComponent>(m_carObject);

	OSK::Vector2f cameraRotation = OSK::Vector2f::Zero;

	// Mouse input
	const OSK::IO::IMouseInput* mouse = nullptr;
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IMouseInput, (const void**)&mouse);

	if (mouse) {
		cameraRotation = {
			static_cast<float>(mouse->GetMouseState().GetPosition().x - mouse->GetPreviousMouseState().GetPosition().x),
			static_cast<float>(mouse->GetMouseState().GetPosition().y - mouse->GetPreviousMouseState().GetPosition().y)
		};
	}

	if (carPhysics.GetAcceleration().GetLenght() > 0.5f * deltaTime ||
		glm::abs(carPhysics.GetVelocity().Dot(carTransform.GetForwardVector())) > 1.2f * deltaTime) {

		const OSK::Vector2f flatCarVec = {
			carTransform.GetRightVector().x,
			carTransform.GetRightVector().z
		};

		const OSK::Vector2f flatArmVec = {
			cameraTransform.GetForwardVector().x,
			cameraTransform.GetForwardVector().z
		};

		const float angle = -flatArmVec.Dot(flatCarVec);
		cameraRotation.x += angle * 750 * deltaTime;
	}

	cameraTransform.RotateWorldSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
	cameraTransform.RotateLocalSpace(glm::radians(cameraRotation.y * 0.25f), { 1, 0, 0 });
}

void CameraAttachmentSystem::SetCar(OSK::ECS::GameObjectIndex carObject) {
	m_carObject = carObject;
}

void CameraAttachmentSystem::SetCamera(OSK::ECS::GameObjectIndex cameraObject) {
	m_cameraObject = cameraObject;
}
