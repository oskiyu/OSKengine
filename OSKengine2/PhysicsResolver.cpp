#include "PhysicsResolver.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "PhysicsComponent.h"
#include "Collider.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void PhysicsResolver::OnTick(TDeltaTime deltaTime) {
	const EntityComponentSystem* ecs = Engine::GetEcs();

	for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
		GameObjectIndex first = event.firstEntity;
		GameObjectIndex second = event.secondEntity;

		// Check.
		if (!(ecs->ObjectHasComponent<PhysicsComponent>(first) && ecs->ObjectHasComponent<PhysicsComponent>(second)))
			continue;

		// --- RESOLUCIÓN INICIAL --- //

		const auto& collisionInfo = event.collisionInfo;
		const Vector3f mtv = collisionInfo.GetMinimumTranslationVector();
		const Vector3f halfMtv = mtv * 0.5f;

		if (collisionInfo.ShouldSwapObjects()) std::swap(first, second);

		auto& transformA = ecs->GetComponent<Transform3D>(first);
		auto& transformB = ecs->GetComponent<Transform3D>(second);

		auto& physicsA = ecs->GetComponent<PhysicsComponent>(first);
		auto& physicsB = ecs->GetComponent<PhysicsComponent>(second);

		const float inverseMassA = physicsA.GetInverseMass();
		const float inverseMassB = physicsB.GetInverseMass();
		const float totalInverseMass = inverseMassA + inverseMassB;

		if (totalInverseMass == 0.0f)
			continue;

		float multiplierA = inverseMassA / totalInverseMass;
		float multiplierB = inverseMassB / totalInverseMass;

		transformA.AddPosition(-halfMtv * multiplierA);
		transformB.AddPosition( halfMtv * multiplierB);


		// --- IMPULSOS --- //

		// Si los objetos se están separando, no ajustaremos las velocidades.
		if (physicsA.GetVelocity().Dot(physicsB.GetVelocity()) < 0.0f)
			continue;

		const Vector3f contactNormal = -collisionInfo.GetMinimumTranslationVector().GetNormalized();

		// Velocidades sin contar la delta de este frame.
		// Para una simulación más estable.
		const Vector3f velocityA = physicsA.GetVelocity() - physicsA.GetCurrentFrameVelocityDelta();
		const Vector3f velocityB = physicsB.GetVelocity() - physicsB.GetCurrentFrameVelocityDelta();

		// Velocidad a la que se separan los objetos.
		const float separationVelocity = -glm::abs(contactNormal.Dot(velocityB - velocityA));

		float cor = (physicsA.coefficientOfRestitution + physicsB.coefficientOfRestitution) * 0.5f;
		cor = glm::mix(cor, 0.0f, separationVelocity * 0.15f);

		// Velocidad a la que se separan los objetos después de la colisión.
		float deltaSeparationVelocity = -(1 + cor) * separationVelocity;
		

		// Cantidad de cambio de velocidad total del sistema 
		// por unidad de impulso.
		const float velocityDeltaPerImpulseUnit = inverseMassA + inverseMassB;

		const float impulsoTotal = deltaSeparationVelocity / velocityDeltaPerImpulseUnit;

		const Vector3f impulsoA =  contactNormal * impulsoTotal;
		const Vector3f impulsoB = -contactNormal * impulsoTotal;

		const Vector3f worldSpaceContactPoint = collisionInfo.GetSingleContactPoint();

		const Vector3f contactPointA = worldSpaceContactPoint - (transformA.GetPosition() + physicsA.centerOfMassOffset);
		const Vector3f contactPointB = worldSpaceContactPoint - (transformB.GetPosition() + physicsB.centerOfMassOffset);

		physicsA.ApplyImpulse(impulsoA, contactPointA);
		physicsB.ApplyImpulse(impulsoB, contactPointB);


		// --- FRICCIÓN --- //

		const Vector3f movementAlongNormalA = physicsA.GetVelocity() - contactNormal * physicsA.GetVelocity().Dot(contactNormal);
		const Vector3f movementAlongNormalB = physicsB.GetVelocity() - contactNormal * physicsB.GetVelocity().Dot(contactNormal);

		physicsA.ApplyImpulse(-movementAlongNormalA * deltaTime, contactPointA);
		physicsB.ApplyImpulse(-movementAlongNormalB * deltaTime, contactPointB);
	}
}
