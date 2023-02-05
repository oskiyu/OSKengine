#include "PhysicsResolver.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void PhysicsResolver::OnTick(TDeltaTime deltaTime) {
	EntityComponentSystem* ecs = Engine::GetEcs();

	for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
		const GameObjectIndex first = event.firstEntity;
		const GameObjectIndex second = event.secondEntity;

		const auto& collisionInfo = event.collisionInfo;
		const Vector3f mtv = collisionInfo.GetMinimumTranslationVector();
		const Vector3f halfMtv = mtv * 0.5f;

		auto& transformA = ecs->GetComponent<Transform3D>(first);
		auto& transformB = ecs->GetComponent<Transform3D>(second);

		transformA.AddPosition(-halfMtv);
		transformB.AddPosition(halfMtv);
	}
}
