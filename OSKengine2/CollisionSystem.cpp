#include "CollisionSystem.h"

#include "CollisionEvent.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Collider.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void CollisionSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Collider>());
	_SetSignature(signature);
}

void CollisionSystem::OnTick(TDeltaTime deltaTime) {
	if (GetObjects().IsEmpty())
		return;

	const TSize numObjects = GetObjects().GetSize();

	for (TIndex a = 0; a < numObjects - 1; a++) {
		const GameObjectIndex firstObject = GetObjects()[a];

		const auto& firstCollider = Engine::GetEcs()->GetComponent<COLLISION::Collider>(firstObject);
		const auto& firstTransform = Engine::GetEcs()->GetComponent<Transform3D>(firstObject);

		for (TIndex b = a + 1; b < numObjects; b++) {
			const GameObjectIndex secondObject = GetObjects()[b];

			if (firstObject == secondObject) continue;

			const auto& secondCollider = Engine::GetEcs()->GetComponent<COLLISION::Collider>(secondObject);
			const auto& secondTransform = Engine::GetEcs()->GetComponent<Transform3D>(secondObject);

			if (firstCollider.GetCollisionInfo(secondCollider, firstTransform, secondTransform).IsTopLevelColliding())
				Engine::GetEcs()->PublishEvent<CollisionEvent>({ firstObject, secondObject });
		}
	}
}
