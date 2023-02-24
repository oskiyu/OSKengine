#include "PhysicsComponent.h"

using namespace OSK;
using namespace OSK::ECS;

void PhysicsComponent::ApplyForce(const Vector3f& force) {
	// Linear motion.
	acceleration += force / mass;
}

void PhysicsComponent::ApplyImpulse(const Vector3f& impulse) {
	velocity += impulse / mass;
}

Vector3f PhysicsComponent::GetMomentum() const {
	return velocity * mass;
}
