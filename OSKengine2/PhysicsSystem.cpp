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
		Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		PhysicsComponent& physicsComponent = Engine::GetEcs()->GetComponent<PhysicsComponent>(obj);

		// physicsComponent.acceleration += gravityAccel * deltaTime * 0.0001f;
		physicsComponent.velocity += physicsComponent.acceleration * deltaTime;
		
		transform.AddPosition(physicsComponent.velocity * deltaTime);
	}
}