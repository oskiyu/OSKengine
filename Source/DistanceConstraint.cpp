#include "DistanceConstraint.h"

#include "PhysicsComponent.h"
#include "TransformComponent3D.h"

void OSK::PHYSICS::DistanceConstraint::Execute(
	ECS::PhysicsComponent* first,
	ECS::TransformComponent3D* firstTransform,
	ECS::PhysicsComponent* second,
	ECS::TransformComponent3D* secondTransform,
	TDeltaTime deltaTime) const
{
	const auto relativePosition = secondTransform->GetTransform().GetPosition() - firstTransform->GetTransform().GetPosition();
	const auto currentDistance = relativePosition.GetLenght();
	const auto normal = relativePosition.GetNormalized();

	// Queremos que la velocidad Va = Vb sólamente en
	// el vector normal de A-B.

	if (currentDistance > m_distance) {
		const auto lambda = 
			(-first->GetVelocity().Dot(relativePosition) - second->GetVelocity().Dot(relativePosition)) /
			(first->GetInverseMass() + second->GetInverseMass());

		first ->ApplyImpulse( normal * lambda, Vector3f::Zero);
		second->ApplyImpulse(-normal * lambda, Vector3f::Zero);
	}
}
