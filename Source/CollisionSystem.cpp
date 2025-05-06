#include "CollisionSystem.h"

#include "CollisionEvent.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "CollisionComponent.h"
#include "TransformComponent3D.h"

// Detectores por defecto.
#include "GjkEpaDetector.h"
#include "GjkClippingDetector.h"
#include "DetailedSphereSphereDetector.h"

#include "AabbAabbDetector.h"
#include "SphereAabbBroadCollisionDetector.h"
#include "SphereSphereBroadCollisionDetector.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void CollisionSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<TransformComponent3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<CollisionComponent>());
	_SetSignature(signature);

	m_broadDispatcher.AddDetector(MakeUnique<AabbAabbCollisionDetector>());
	m_broadDispatcher.AddDetector(MakeUnique<SphereAabbCollisionDetector>());
	m_broadDispatcher.AddDetector(MakeUnique<SphereSphereBroadCollisionDetector>());

	m_narrowDispatcher.AddDetector(MakeUnique<GjkEpaDetector>());
	m_narrowDispatcher.AddDetector(MakeUnique<GjkClippingDetector>());
	m_narrowDispatcher.AddDetector(MakeUnique<DetailedSphereSphereDetector>());
}

void CollisionSystem::OnExecutionStart() {
	// Actualizar y transformar los colliders.
	for (const GameObjectIndex i : GetAllCompatibleObjects()) {
		auto& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(i).GetCollider();
		const auto& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(i).GetTransform();

		collider.GetTopLevelCollider()->UpdatePosition(transform.GetPosition());

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

		for (UIndex64 b = indexInGlobalList + 1; b < GetAllCompatibleObjects().size(); b++) {

			const GameObjectIndex second = GetAllCompatibleObjects()[b];
			const auto& secondCollider = *Engine::GetEcs()->GetComponent<CollisionComponent>(second).GetCollider();

			if ( m_broadDispatcher.GetCollision(*firstCollider.GetTopLevelCollider(), *secondCollider.GetTopLevelCollider())) {
				ProcessColliderPair(first, second, firstCollider, secondCollider);
			}
		}
	}
}

void CollisionSystem::ProcessColliderPair(
	GameObjectIndex firstObj,
	GameObjectIndex secondObj,
	const COLLISION::Collider& first, 
	const COLLISION::Collider& second) const 
{
	for (UIndex64 a = 0; a < first.GetBottomLevelCollidersCount(); a++) {
		for (UIndex64 b = 0; b < second.GetBottomLevelCollidersCount(); b++) {
			const auto& firstBlc  = *first.GetBottomLevelCollider(a);
			const auto& secondBlc = *second.GetBottomLevelCollider(b);

			const auto collision = m_narrowDispatcher.GetCollision(firstBlc, secondBlc);

			if (collision.IsColliding()) {
				Engine::GetEcs()->PublishEvent<CollisionEvent>({
						firstObj,
						secondObj,
						collision
					});
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

		const auto topLevelResult = collider.GetTopLevelCollider()->GetCollider()->CastRay(ray);

		if (!topLevelResult.Result()) {
			// continue;
		}

		for (UIndex32 blc_i = 0; blc_i < collider.GetBottomLevelCollidersCount(); blc_i++) {
			const auto& blc = collider.GetBottomLevelCollider(blc_i);
			const auto result = blc->GetCollider()->CastRay(ray);

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

void CollisionSystem::AddBroadCollisionDetector(UniquePtr<COLLISION::IBroadCollisionDetector>&& detector) {
	m_broadDispatcher.AddDetector(std::move(detector));
}

void CollisionSystem::AddDetailedCollisionDetector(UniquePtr<IDetailedCollisionDetector>&& detector) {
	m_narrowDispatcher.AddDetector(std::move(detector));
}
