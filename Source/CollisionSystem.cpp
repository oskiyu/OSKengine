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

void CollisionSystem::OnExecutionStart() {
	// Actualizar y transformar los colliders.
	for (const GameObjectIndex i : GetAllCompatibleObjects()) {
		auto& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(i).GetCollider();
		const auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(i);

		collider.GetTopLevelCollider()->SetPosition(transform.GetPosition());

		for (UIndex32 blci = 0; blci < collider.GetBottomLevelCollidersCount(); blci++) {
			auto* blc = collider.GetBottomLevelCollider(blci);

			blc->Transform(transform);
		}
	}
}

void CollisionSystem::Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) {
	if (GetAllCompatibleObjects().size() < 2) {
		return;
	}

	const USize64 numObjects = objects.size();
	
	for (UIndex64 a = 0; a < numObjects; a++) {
		UIndex64 indexInGlobalList = objects.data() - GetAllCompatibleObjects().data() + a;

		const GameObjectIndex first = objects[a];

		const auto& firstCollider = *Engine::GetEcs()->GetComponent<CollisionComponent>(first).GetCollider();
		const auto& firstTransform = Engine::GetEcs()->GetComponent<Transform3D>(first);

		for (UIndex64 b = indexInGlobalList + 1; b < GetAllCompatibleObjects().size(); b++) {

			const GameObjectIndex second = GetAllCompatibleObjects()[b];

			const auto& secondCollider = *Engine::GetEcs()->GetComponent<CollisionComponent>(second).GetCollider();
			const auto& secondTransform = Engine::GetEcs()->GetComponent<Transform3D>(second);

			const auto collisionInfo = firstCollider.GetCollisionInfo(secondCollider, firstTransform, secondTransform);

			if (collisionInfo.IsColliding()) {

				for (const auto& detailedCollision : collisionInfo.GetDetailedInfo()) {
					Engine::GetEcs()->PublishEvent<CollisionEvent>({
						first,
						second,
						detailedCollision
						});
				}

// #define OSK_COLLISION_DEBUG
#ifdef OSK_COLLISION_DEBUG
				Engine::GetLogger()->DebugLog(std::format("Collision: {} - {}", first, second));
				Engine::GetLogger()->DebugLog(std::format("\tFrame: {}", Engine::GetCurrentGameFrameIndex()));
				for (const auto& c : collisionInfo.GetDetailedInfo()) {
					Engine::GetLogger()->DebugLog(std::format("\tWolrd points: {:.3f} {:.3f} {:.3f}",
						c.GetSingleContactPoint().x,
						c.GetSingleContactPoint().y,
						c.GetSingleContactPoint().z));
				}
				// Engine::GetLogger()->DebugLog(std::format("\tPoints count: {}", collisionInfo.GetDetailedInfo().GetContactPoints().GetSize()));
				// for (const auto& p : collisionInfo.GetDetailedInfo().GetContactPoints())
				//	Engine::GetLogger()->DebugLog(std::format("\t\t{:.3f} {:.3f} {:.3f}", p.x, p.y, p.z));
#endif

			}
		}
	}
}

RayCastResult CollisionSystem::CastRay(const Ray& ray, GameObjectIndex sendingObject) const {
	float closestDistance = std::numeric_limits<float>::max();
	RayCastResult closest = RayCastResult::False();

	for (const GameObjectIndex i : GetAllCompatibleObjects()) {
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
