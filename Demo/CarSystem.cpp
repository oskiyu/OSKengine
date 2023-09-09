#include "CarSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/Math.h>

#include "CarInputEvent.h"
#include "CarComponent.h"

void CarSystem::OnCreate() {
	OSK::ECS::Signature signature{};

	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::PhysicsComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::Transform3D>());

	this->_SetSignature(signature);
}

void CarSystem::OnTick(TDeltaTime deltaTime) {
	const auto* ecs = OSK::Engine::GetEcs();
	const auto& eventQueue = ecs->GetEventQueue<CarInputEvent>();

	for (const auto obj : GetObjects()) {
		auto& physicsComponent = ecs->GetComponent<OSK::ECS::PhysicsComponent>(obj);
		auto& transform = ecs->GetComponent<OSK::ECS::Transform3D>(obj);

		float currentAngle = 0.0f;

		// Procesar inputs
		for (const auto& [type, id] : eventQueue) {
			if (id == obj) {
				switch (type) {
				case CarInputEvent::Type::ACCELERATE:
					physicsComponent.ApplyForce( transform.GetForwardVector() * 5.0f);
					break;

				case CarInputEvent::Type::DECELERATE:
					physicsComponent.ApplyForce(-transform.GetForwardVector() * 5.0f);
					break;

				case CarInputEvent::Type::TURN_LEFT:
					currentAngle -= 45.0f;
					break;

				case CarInputEvent::Type::TURN_RIGHT:
					currentAngle += 45.0f;
					break;
				}
			}
		}

		// Fuerzas laterales
		float carLenght = 1.5f;

		const OSK::Vector3f front = transform.GetForwardVector();
		const OSK::Vector3f right = transform.GetRightVector();
		const OSK::Vector3f up = transform.GetTopVector();
		const OSK::Vector3f velocity = physicsComponent.GetVelocity();
		const float speed = velocity.GetLenght();


		// Proyectamos el vector velocidad al plano de referencia (suelo del coche).
		const float velocityVerticalProjection = velocity.Dot(up);
		const OSK::Vector3f carSpaceVelocity = velocity - up * velocityVerticalProjection;

		// Hacia la derecha
		const float vLateral = carSpaceVelocity.Dot(right);

		const float carAngle = glm::radians(carSpaceVelocity.GetAngle(front)) * glm::sign(vLateral);

		// Ángulo de las ruedas delanteras (en radianes).
		const float angleFront = carAngle + glm::radians(currentAngle);
		// Ángulo de las ruedas traseras (en radianes).
		const float angleRear = carAngle;

		const float coeficienteAgarre = -3.0f;

		const float fuerzaLateralDelantera = coeficienteAgarre * angleFront;
		const float fuerzaLateralTrasera = coeficienteAgarre * angleRear;

		const float fLateral = coeficienteAgarre * vLateral * deltaTime * physicsComponent.GetMass();
		// if (glm::abs(fLateral) > 0.01f)  physicsComponent.ApplyForce(right * fLateral * 100);

		const float torque = fuerzaLateralDelantera - fuerzaLateralTrasera;


		const float finalAngleFast = torque;

		// const float finalAngle = glm::mix(finalAngleSlow, finalAngleFast, velocity.GetLenght() * 3);
		if (physicsComponent.GetVelocity().GetLenght() > 0.1f && glm::abs(torque) > 0.01f)
			transform.RotateWorldSpace(finalAngleFast * deltaTime, { 0.0f, 1.0f, 0.0f });
	}

}
