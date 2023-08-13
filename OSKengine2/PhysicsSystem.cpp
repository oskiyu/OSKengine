#include "PhysicsSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "PhysicsComponent.h"

using namespace OSK;
using namespace OSK::ECS;

void PhysicsSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<PhysicsComponent>());
	_SetSignature(signature);
}

void PhysicsSystem::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		auto& physicsComponent = Engine::GetEcs()->GetComponent<PhysicsComponent>(obj);

		if (physicsComponent.IsImmovable())
			continue;

		const auto velocity = physicsComponent.GetVelocity();
		const auto angularVelocity = physicsComponent.GetAngularVelocity();
		const auto acceleration = physicsComponent.GetAcceleration();

		transform.AddPosition(velocity * deltaTime);
		physicsComponent._SetVelocity(velocity + (acceleration + Vector3f(0, -2.6f, 0)) * deltaTime);
		
		if (physicsComponent.GetAngularVelocity().GetLenght() > 0.00001f) {
			transform.RotateWorldSpace(
				physicsComponent.GetAngularVelocity().GetLenght() * deltaTime,
				physicsComponent.GetAngularVelocity().GetNormalized()
			);

			physicsComponent._SetAngularVelocity(angularVelocity * (1.0f - (0.9f * deltaTime)));
		}

		physicsComponent._ResetForces();
		physicsComponent._ResetCurrentFrameDeltas();
	}
}
