#include "CollisionSystem.h"

#include "CollisionEvent.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "CollisionComponent.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void CollisionSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<CollisionComponent>());
	_SetSignature(signature);
}

void CollisionSystem::OnTick(TDeltaTime deltaTime) {
	if (GetObjects().GetSize() < 2)
		return;

	const USize64 numObjects = GetObjects().GetSize();

	// Actualizar y transformar los colliders.
	for (GameObjectIndex i : GetObjects()) {
		auto& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(i).GetCollider();
		const auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(i);

		collider.GetTopLevelCollider()->SetPosition(transform.GetPosition());

		for (UIndex32 blci = 0; blci < collider.GetBottomLevelCollidersCount(); blci++) {
			auto* blc = collider.GetBottomLevelCollider(blci);

			blc->Transform(transform);
		}
	}

	// Detección de colisiones.
	for (UIndex64 a = 0; a < numObjects - 1; a++) {
		const GameObjectIndex firstObject = GetObjects()[a];

		const auto& firstCollider = *Engine::GetEcs()->GetComponent<CollisionComponent>(firstObject).GetCollider();
		const auto& firstTransform = Engine::GetEcs()->GetComponent<Transform3D>(firstObject);

		for (UIndex64 b = a + 1; b < numObjects; b++) {
			const GameObjectIndex secondObject = GetObjects()[b];

			const auto& secondCollider = *Engine::GetEcs()->GetComponent<CollisionComponent>(secondObject).GetCollider();
			const auto& secondTransform = Engine::GetEcs()->GetComponent<Transform3D>(secondObject);

			const auto collisionInfo = firstCollider.GetCollisionInfo(secondCollider, firstTransform, secondTransform);

			if (collisionInfo.IsColliding()) {

				for (const auto& detailedCollision : collisionInfo.GetDetailedInfo()) {
					Engine::GetEcs()->PublishEvent<CollisionEvent>({
						firstObject,
						secondObject,
						detailedCollision
						});
				}

// #define OSK_COLLISION_DEBUG
#ifdef OSK_COLLISION_DEBUG
				Engine::GetLogger()->DebugLog(std::format("Collision: {} - {}", firstObject, secondObject));
				Engine::GetLogger()->DebugLog(std::format("\tFrame: {}", Engine::GetCurrentGameFrameIndex()));
				Engine::GetLogger()->DebugLog(std::format("\tWolrd point: {:.3f} {:.3f} {:.3f}", 
					collisionInfo.GetDetailedInfo().GetSingleContactPoint().x,
					collisionInfo.GetDetailedInfo().GetSingleContactPoint().y,
					collisionInfo.GetDetailedInfo().GetSingleContactPoint().z));
				Engine::GetLogger()->DebugLog(std::format("\tPoints count: {}", collisionInfo.GetDetailedInfo().GetContactPoints().GetSize()));
				for (const auto& p : collisionInfo.GetDetailedInfo().GetContactPoints())
					Engine::GetLogger()->DebugLog(std::format("\t\t{:.3f} {:.3f} {:.3f}", p.x, p.y, p.z));
#endif

			}
		}
	}
}

RayCastResult CollisionSystem::CastRay(const Ray& ray, GameObjectIndex sendingObject) const {
	float closestDistance = std::numeric_limits<float>::max();
	RayCastResult closest = RayCastResult::False();

	for (GameObjectIndex i : GetObjects()) {
		if (i == sendingObject) {
			continue;
		}

		const auto& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(i).GetCollider();

		const auto topLevelResult = collider.GetTopLevelCollider()->CastRay(ray);

		if (!topLevelResult.Result()) {
			// continue;
		}

		for (UIndex32 blc_i = 0; blc_i < collider.GetBottomLevelCollidersCount(); blc_i++) {
			const auto& blc = collider.GetBottomLevelCollider(blc_i);
			const auto result = blc->CastRay(ray);

			if (!result.Result()) {
				continue;
			}

			const float nDistance = result.GetIntersectionPoint().GetDistanceTo2(ray.origin);
			if (nDistance < closestDistance) {
				closestDistance = nDistance;
				closest = RayCastResult::True(result.GetIntersectionPoint(), i);
			}
		}
	}

	return closest;
}
