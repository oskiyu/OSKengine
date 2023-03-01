#include "PhysicsResolver.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "PhysicsComponent.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void PhysicsResolver::OnTick(TDeltaTime deltaTime) {
	EntityComponentSystem* ecs = Engine::GetEcs();

	for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
		const GameObjectIndex first = event.firstEntity;
		const GameObjectIndex second = event.secondEntity;

		// Check.
		if (!(ecs->ObjectHasComponent<PhysicsComponent>(first) && ecs->ObjectHasComponent<PhysicsComponent>(second)))
			continue;

		const auto& collisionInfo = event.collisionInfo;
		const Vector3f mtv = collisionInfo.GetMinimumTranslationVector();
		const Vector3f halfMtv = mtv * 0.5f;

		auto& transformA = ecs->GetComponent<Transform3D>(first);
		auto& transformB = ecs->GetComponent<Transform3D>(second);

		auto& physicsA = ecs->GetComponent<PhysicsComponent>(first);
		auto& physicsB = ecs->GetComponent<PhysicsComponent>(second);

		return;

		transformA.AddPosition(-halfMtv);
		transformB.AddPosition( halfMtv);


		const Vector3f aToB = mtv.GetNormalized();

		const Vector3f impulseA = -aToB * physicsB.GetMomentum().GetLenght();
		const Vector3f impulseB =  aToB * physicsA.GetMomentum().GetLenght();

		physicsA.ApplyLinealImpulse(impulseA);
		physicsB.ApplyLinealImpulse(impulseB);

		const Vector3f worldSpaceContactPoint = collisionInfo.GetSingleContactPoint();
		const Vector3f contactPointA = worldSpaceContactPoint - (transformA.GetPosition() + physicsA.centerOfMassOffset);
		const Vector3f contactPointB = worldSpaceContactPoint - (transformB.GetPosition() + physicsB.centerOfMassOffset);

		physicsA.ApplyAngularImpulse(impulseA, contactPointA);
		physicsB.ApplyAngularImpulse(impulseB, contactPointB);
	}
}
