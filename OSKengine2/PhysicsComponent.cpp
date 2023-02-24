#include "PhysicsComponent.h"

using namespace OSK;
using namespace OSK::ECS;

void PhysicsComponent::ApplyForce(const Vector3f& force) {
	// Linear motion.
	acceleration += force / mass;
}

void PhysicsComponent::ApplyLinealImpulse(const Vector3f& impulse) {
	velocity += impulse / mass;
}

void PhysicsComponent::ApplyAngularImpulse(const Vector3f& impulse, const Vector3f& localSpacePoint) {
	Vector3f torque = GetTorque(localSpacePoint, impulse);
	
	angularVelocity += torque;
}

Vector3f PhysicsComponent::GetTorque(const Vector3f& localSpacePoint, const Vector3f& force) const {
	return localSpacePoint.Cross(force);
}

Vector3f PhysicsComponent::GetMomentum() const {
	return velocity * mass;
}
