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
		GameObjectIndex first  = event.firstEntity;
		GameObjectIndex second = event.secondEntity;

		// Check.
		if (!(ecs->ObjectHasComponent<PhysicsComponent>(first) && ecs->ObjectHasComponent<PhysicsComponent>(second)))
			continue;

		// --- RESOLUCIÓN INICIAL --- //

		const auto& collisionInfo = event.collisionInfo;

		Vector3f contactNormal = -collisionInfo.GetFirstFaceNormal().GetNormalized();

		const Vector3f mtv = collisionInfo.GetMinimumTranslationVector();
		const Vector3f halfMtv = mtv * 0.5f;

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

		// Velocidades sin contar la delta de este frame.
		// Para una simulación más estable.
		const Vector3f velocityA = physicsA.GetVelocity() - physicsA.GetCurrentFrameVelocityDelta();
		const Vector3f velocityB = physicsB.GetVelocity() - physicsB.GetCurrentFrameVelocityDelta();

		// Velocidad a la que se separan los objetos.
		const float projectedVelocityA = glm::abs(velocityA.Dot(contactNormal));
		const float projectedVelocityB = glm::abs(velocityB.Dot(contactNormal));
		const float separationVelocity = -glm::abs(projectedVelocityA - projectedVelocityB);

		float cor = (physicsA.coefficientOfRestitution + physicsB.coefficientOfRestitution) * 0.5f;
		/*
		cor = glm::clamp(
			glm::mix(0.0f, cor, glm::abs(separationVelocity) * 2.55f - 0.5f),
			0.0f,
			cor
		);
		*/

		// Velocidad a la que se separan los objetos después de la colisión.
		float deltaSeparationVelocity = -(1 + cor) * separationVelocity;
		

		// Cantidad de cambio de velocidad total del sistema 
		// por unidad de impulso.
		const float velocityDeltaPerImpulseUnit = inverseMassA + inverseMassB;

		const float impulsoTotal = deltaSeparationVelocity / velocityDeltaPerImpulseUnit;

		const Vector3f impulsoA = contactNormal * impulsoTotal;
		const Vector3f impulsoB = -impulsoA;

		const Vector3f worldSpaceContactPoint = collisionInfo.GetSingleContactPoint();

		const Vector3f centerOfMassA = transformA.GetPosition() + physicsA.centerOfMassOffset;
		const Vector3f centerOfMassB = transformB.GetPosition() + physicsB.centerOfMassOffset;

		const Vector3f contactPointA = worldSpaceContactPoint - centerOfMassA;
		const Vector3f contactPointB = worldSpaceContactPoint - centerOfMassB;

		physicsA.ApplyImpulse(impulsoA, contactPointA);
		physicsB.ApplyImpulse(impulsoB, contactPointB);


		// --- FRICCIÓN --- //

#if 0 // Fircción direccionada.
		const Vector4f transformFrictionA = transformA.GetRotation().ToMat4() * glm::vec4(physicsA.localFrictionCoefficient.ToGLM(), 1.0f);
		const Vector4f transformFrictionB = transformB.GetRotation().ToMat4() * glm::vec4(physicsB.localFrictionCoefficient.ToGLM(), 1.0f);

		const Vector3f transformedFrictionA = glm::abs(transformA.GetRotation().RotateVector(physicsA.localFrictionCoefficient).ToGLM());
		const Vector3f transformedFrictionB = glm::abs(transformB.GetRotation().RotateVector(physicsB.localFrictionCoefficient).ToGLM());
		/* = glm::abs(transformA.GetForwardVector().ToGLM()) * physicsA.localFrictionCoefficient.x
			+ glm::abs(transformA.GetTopVector().ToGLM())	  * physicsA.localFrictionCoefficient.y
			+ glm::abs(transformA.GetRightVector().ToGLM())	  * physicsA.localFrictionCoefficient.Z;
		const Vector3f transformedFrictionB
			= glm::abs(transformB.GetForwardVector().ToGLM()) * physicsB.localFrictionCoefficient.x
			+ glm::abs(transformB.GetTopVector().ToGLM())	  * physicsB.localFrictionCoefficient.y
			+ glm::abs(transformB.GetRightVector().ToGLM())   * physicsB.localFrictionCoefficient.Z; */
#endif

		const Vector3f movementAlongNormalA = 
			(physicsA.GetVelocity() - contactNormal * physicsA.GetVelocity().Dot(contactNormal));
		const Vector3f movementAlongNormalB = 
			(physicsB.GetVelocity() - contactNormal * physicsB.GetVelocity().Dot(contactNormal));

		physicsA.ApplyImpulse(-movementAlongNormalA * deltaTime, contactPointA);
		physicsB.ApplyImpulse(-movementAlongNormalB * deltaTime, contactPointB);

		continue;


		// ---- SNAPPING --- //

		const Vector3f firstNormal = collisionInfo.GetFirstFaceNormal();
		const Vector3f secondNormal = collisionInfo.GetSecondFaceNormal();
		const Vector3f cross = firstNormal.Cross(-secondNormal);
		const float angle = glm::radians(firstNormal.GetAngle(secondNormal));

		if (glm::abs(angle) > 0.1f) {
			transformA.RotateWorldSpace(
				angle * multiplierA,
				cross.GetNormalized()
			);
			transformB.RotateWorldSpace(
				-angle * multiplierB,
				cross.GetNormalized()
			);
		}
	}
}
