#include "CarSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/Math.h>

#include "CarComponent.h"
#include "CarControllerComponent.h"
#include "EngineComponent.h"

void CarSystem::OnCreate() {
	OSK::ECS::Signature signature{};

	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CarControllerComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<EngineComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::PhysicsComponent>());
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<OSK::ECS::Transform3D>());

	this->_SetSignature(signature);
}

void CarSystem::OnTick(TDeltaTime deltaTime) {
	const auto* ecs = OSK::Engine::GetEcs();

	for (const auto obj : GetObjects()) {
		auto& physicsComponent = ecs->GetComponent<OSK::ECS::PhysicsComponent>(obj);
		auto& transform = ecs->GetComponent<OSK::ECS::Transform3D>(obj);
		const auto& controller = ecs->GetComponent<CarControllerComponent>(obj);
		auto& carComponent = ecs->GetComponent<CarComponent>(obj);

		const OSK::Vector3f front = transform.GetForwardVector();
		const OSK::Vector3f right = transform.GetRightVector();
		const OSK::Vector3f up = transform.GetTopVector();

		float currentAngle = controller.rightTurn * carComponent.maxTurning;
		float acceleration = controller.acceleration > 0.0f ? 1.0f : -1.65f;

		const float currentAngleDiff = glm::abs(currentAngle - carComponent.previousFrameTurning);
		if (currentAngleDiff / deltaTime > carComponent.maxTurningPerSecond) {
			currentAngle = glm::mix(
				carComponent.previousFrameTurning, 
				currentAngle, 
				currentAngleDiff / (carComponent.maxTurningPerSecond));
		}

		const bool isOnFloor = up.Dot(OSK::Vector3f(0, 1, 0)) > 0.96f;

		// Motor
		{
			auto& motor = ecs->GetComponent<EngineComponent>(obj);

			const float wheelRadius = 0.5f;
			const float carLinearVelocity = physicsComponent.GetVelocity().GetLenght();

			// Rad/s
			const float wheelAngularVelocity = carLinearVelocity / wheelRadius;
			motor.currentRpm = wheelAngularVelocity * motor.GetCurrentTorqueMultiplier() * 25.0f;

			if (motor.currentRpm > motor.maxRpm && motor.currentGear < motor.gears.size()) {
				motor.currentGear++;
				motor.currentRpm -= motor.changePoint;
			}
			else if (motor.currentRpm < motor.changePoint && motor.currentGear > 0) {
				motor.currentGear--;
				motor.currentRpm += motor.changePoint;
			}
			
			if (isOnFloor) {
				const float torque = 1.0f * glm::max(motor.currentRpm, motor.minRpm) * motor.GetCurrentTorqueMultiplier() / wheelRadius * acceleration * 50.0f;
				physicsComponent.ApplyForce(transform.GetForwardVector() * torque / 550.0f);
			}

			motor.audioSource->SetPitch(motor.currentRpm / motor.minRpm);
		}

		// Fuerzas laterales
		float carLenght = 1.5f;

		const OSK::Vector3f velocity = physicsComponent.GetVelocity();
		const float speed = velocity.GetLenght();

		// Proyectamos el vector velocidad al plano de referencia (suelo del coche).
		const float velocityVerticalProjection = velocity.Dot(OSK::Vector3f(0, 1, 0));
		const OSK::Vector3f carSpaceVelocity = velocity - OSK::Vector3f(0, 1, 0) * velocityVerticalProjection;

		// Hacia la derecha
		const float vLateral = carSpaceVelocity.Dot(right);

		const float carAngle = glm::radians(carSpaceVelocity.GetAngle(front)) * glm::sign(vLateral);

		// Ángulo de las ruedas delanteras (en radianes).
		const float angleFront = carAngle + glm::radians(currentAngle);
		// Ángulo de las ruedas traseras (en radianes).
		const float angleRear = carAngle;

		const float coeficienteAgarre = -7.0f;

		const float fuerzaLateralDelantera = coeficienteAgarre * angleFront;
		const float fuerzaLateralTrasera = coeficienteAgarre * angleRear;

		const float fLateral = coeficienteAgarre * vLateral * deltaTime * physicsComponent.GetMass();
		if (isOnFloor && glm::abs(fLateral) > 0.01f)  physicsComponent.ApplyForce(right * fLateral * 100);

		const float torque = fuerzaLateralDelantera - fuerzaLateralTrasera;

		const float finalAngleFast = torque;

		// const float finalAngle = glm::mix(finalAngleSlow, finalAngleFast, velocity.GetLenght() * 3);
		if (physicsComponent.GetVelocity().GetLenght() > 0.01f && glm::abs(torque) > 0.01f)
			transform.RotateWorldSpace(finalAngleFast * deltaTime, { 0.0f, 1.0f, 0.0f });


		// Girar ruedas
		auto& frontLeft = ecs->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelFrontLeft);
		auto& frontRight = ecs->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelFrontRight);
		auto& backLeft = ecs->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelBackLeft);
		auto& backRight = ecs->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelBackRight);

		const float difference = carComponent.previousFrameTurning - currentAngle;

		if (glm::abs(difference) > 0.0001f) {
			frontLeft.RotateWorldSpace(glm::radians(difference), OSK::Vector3f(0.0f, 1.0f, 0.0f));
			frontRight.RotateWorldSpace(glm::radians(difference), OSK::Vector3f(0.0f, 1.0f, 0.0f));
		}

		carComponent.previousFrameTurning = currentAngle;

		if (speed > 0.01f) {
			const float multiplier = 15.f;

			frontLeft.RotateLocalSpace(speed * multiplier * deltaTime, OSK::Vector3f(1.0f, 0.0f, 0.0f));
			frontRight.RotateLocalSpace(speed * multiplier * deltaTime, OSK::Vector3f(-1.0f, 0.0f, 0.0f));
			backLeft.RotateLocalSpace(speed * multiplier * deltaTime, OSK::Vector3f(1.0f, 0.0f, 0.0f));
			backRight.RotateLocalSpace(speed * multiplier * deltaTime, OSK::Vector3f(-1.0f, 0.0f, 0.0f));
		}
	}

}
