#include "PhysicsComponent.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::ECS;


void PhysicsComponent::SetImmovable() {
	SetInverseMass(0.0f);
}

bool PhysicsComponent::IsImmovable() const {
	return inverseMass == 0.0f;
}

void PhysicsComponent::SetMass(float newMass) {
	OSK_ASSERT(newMass > 0.0f, InvalidArgumentException("La masa debe ser mayor que 0."));

	mass = newMass;
	inverseMass = 1.0f / mass;
}

void PhysicsComponent::SetInverseMass(float newInverseMass) {
	inverseMass = newInverseMass;
	mass = inverseMass == 0.0f
		? std::numeric_limits<float>::infinity()
		: 1.0f / inverseMass;
}

float PhysicsComponent::GetMass() const {
	return mass;
}

float PhysicsComponent::GetInverseMass() const {
	return inverseMass;
}

glm::mat3 PhysicsComponent::GetInertiaTensor() const {
	return inertiaTensor;
}

glm::mat3 PhysicsComponent::GetInverseInertiaTensor() const {
	return inverseInertiaTensor;
}

Vector3f PhysicsComponent::GetVelocity() const {
	return velocity;
}

Vector3f PhysicsComponent::GetCurrentFrameVelocityDelta() const {
	return currentFrameVelocityDelta;
}

Vector3f PhysicsComponent::GetVelocityOfPoint(const Vector3f& localSpacePoint) const {
	return velocity + angularVelocity.Cross(localSpacePoint);
}

Vector3f PhysicsComponent::GetAcceleration() const {
	return acceleration;
}

Vector3f PhysicsComponent::GetAngularVelocity() const {
	return angularVelocity;
}

void PhysicsComponent::_ResetForces() {
	acceleration = Vector3f::Zero;
}

void PhysicsComponent::_ResetCurrentFrameDeltas() {
	currentFrameVelocityDelta = Vector3f::Zero;
}

void PhysicsComponent::ApplyForce(const Vector3f& force) {
	// Linear motion.
	_SetAcceleration(acceleration + force * GetInverseMass());
}

void PhysicsComponent::ApplyImpulse(const Vector3f& impulse, const Vector3f& localSpacePoint) {
	Vector3f torque = GetTorque(localSpacePoint, impulse);

	_SetVelocity(velocity + impulse * GetInverseMass());
	_SetAngularVelocity(angularVelocity + Vector3f(torque.ToGlm() * inverseInertiaTensor));
}

Vector3f PhysicsComponent::GetTorque(const Vector3f& localSpacePoint, const Vector3f& force) {
	return localSpacePoint.Cross(force);
}

void PhysicsComponent::_SetVelocity(Vector3f velocity) {
	currentFrameVelocityDelta += this->velocity - velocity;
	this->velocity = velocity;
}

void PhysicsComponent::_SetAngularVelocity(Vector3f angularVelocity) {
	this->angularVelocity = angularVelocity;
}

void PhysicsComponent::_SetAcceleration(Vector3f acceleration) {
	this->acceleration = acceleration;
}
