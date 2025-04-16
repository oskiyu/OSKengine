#include "PhysicsResolver.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "TransformComponent3D.h"
#include "PhysicsComponent.h"
#include "Collider.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void PhysicsResolver::Execute(TDeltaTime deltaTime, std::span<const CollisionEvent> events) {
	EntityComponentSystem* ecs = Engine::GetEcs();

	for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
		GameObjectIndex first  = event.firstEntity;
		GameObjectIndex second = event.secondEntity;

		// Check.
		if (!(ecs->ObjectHasComponent<PhysicsComponent>(first) && ecs->ObjectHasComponent<PhysicsComponent>(second)))
			continue;

		auto& transformA = ecs->GetComponent<TransformComponent3D>(first).GetTransform();
		auto& transformB = ecs->GetComponent<TransformComponent3D>(second).GetTransform();

		auto& physicsA = ecs->GetComponent<PhysicsComponent>(first);
		auto& physicsB = ecs->GetComponent<PhysicsComponent>(second);


		// --- RESOLUCIÓN INICIAL --- //

		const auto& collisionInfo = event.collisionInfo;

#ifdef OSK_COLLISION_DEBUG
		
		Engine::GetLogger()->DebugLog(std::format("Detailed collision: {} - {}", first, second));
		Engine::GetLogger()->DebugLog(std::format("\tFrame: {}", Engine::GetCurrentGameFrameIndex()));
		Engine::GetLogger()->DebugLog(std::format("\tWolrd points: {:.3f} {:.3f} {:.3f}",
			collisionInfo.GetSingleContactPoint().x,
			collisionInfo.GetSingleContactPoint().y,
			collisionInfo.GetSingleContactPoint().z));

#endif // OSK_COLLISION_DEBUG

		if (collisionInfo.GetMinimumTranslationVector().GetLenght() < 0.001f) {
			continue;
		}


		const Vector3f aToB = collisionInfo.GetFirstFaceNormal();
		const Vector3f bToA = collisionInfo.GetSecondFaceNormal();
		
		Vector3f contactNormal = collisionInfo.GetMinimumTranslationVector().GetNormalized();

		const Vector3f mtv = collisionInfo.GetMinimumTranslationVector();

		const float inverseMassA = physicsA.GetInverseMass();
		const float inverseMassB = physicsB.GetInverseMass();
		const float totalInverseMass = inverseMassA + inverseMassB;

		if (totalInverseMass == 0.0f)
			continue;

		float multiplierA = inverseMassA / totalInverseMass;
		float multiplierB = inverseMassB / totalInverseMass;

		transformA.AddPosition_ThreadSafe(-mtv * multiplierA);
		transformB.AddPosition_ThreadSafe( mtv * multiplierB);
		

		// --- IMPULSOS --- //
		
		// Velocidades sin contar la delta de este frame.
		// Para una simulación más estable.
		const Vector3f velocityA = physicsA.GetVelocity() - physicsA.GetCurrentFrameVelocityDelta();
		const Vector3f velocityB = physicsB.GetVelocity() - physicsB.GetCurrentFrameVelocityDelta();

		// Velocidad a la que se separan los objetos.
		const float projectedVelocityA = velocityA.Dot(-contactNormal);
		const float projectedVelocityB = velocityB.Dot(-contactNormal);

		const float separationVelocity = -(projectedVelocityA - projectedVelocityB);
		if (separationVelocity < 0.0f) {
			continue;
		}

		float cor = (physicsA.coefficientOfRestitution + physicsB.coefficientOfRestitution) * 0.5f;
		
		cor = glm::clamp(
			glm::mix(0.0f, cor, glm::abs(separationVelocity) * 2.55f - 0.5f),
			0.0f,
			cor
		);
		

		// Velocidad a la que se separan los objetos después de la colisión.
		float deltaSeparationVelocity = -(1 + cor) * separationVelocity;

		// Cantidad de cambio de velocidad total del sistema 
		// por unidad de impulso.
		const float velocityDeltaPerImpulseUnit = inverseMassA + inverseMassB;

		const float impulsoTotal = deltaSeparationVelocity / velocityDeltaPerImpulseUnit;

		const Vector3f impulsoA =  contactNormal * impulsoTotal * multiplierA;
		const Vector3f impulsoB = -contactNormal * impulsoTotal * multiplierB;

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

		const float frictionCoefficient = (physicsA.frictionCoefficient + physicsB.frictionCoefficient) * 0.5f;

		const Vector3f movementAlongNormalA = 
			(physicsA.GetVelocity() - contactNormal * physicsA.GetVelocity().Dot(contactNormal));
		const Vector3f movementAlongNormalB = 
			(physicsB.GetVelocity() - contactNormal * physicsB.GetVelocity().Dot(contactNormal));

		physicsA.ApplyImpulse(-movementAlongNormalA * deltaTime * frictionCoefficient, contactPointA);
		physicsB.ApplyImpulse(-movementAlongNormalB * deltaTime * frictionCoefficient, contactPointB);

		continue;


		// ---- SNAPPING --- //

		const Vector3f firstNormal = collisionInfo.GetFirstFaceNormal();
		const Vector3f secondNormal = collisionInfo.GetSecondFaceNormal();
		const Vector3f cross = firstNormal.Cross(-secondNormal);
		const float angle = glm::radians(firstNormal.GetAngle(secondNormal));

		if (glm::abs(angle) > 0.1f) {
			transformA.RotateWorldSpace_ThreadSafe(
				angle * multiplierA,
				cross.GetNormalized()
			);
			transformB.RotateWorldSpace_ThreadSafe(
				-angle * multiplierB,
				cross.GetNormalized()
			);
		}
	}
}
