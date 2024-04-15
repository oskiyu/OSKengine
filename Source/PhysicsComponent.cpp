#include "PhysicsComponent.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

#include "Math.h"


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

void PhysicsComponent::SetInertiaTensor(const glm::mat3& tensor) {
	inertiaTensor = tensor;
	inverseInertiaTensor = glm::inverse(tensor);
}

void PhysicsComponent::SetInverseInertiaTensor(const glm::mat3& inverseTensor) {
	inertiaTensor = glm::inverse(inverseTensor);
	inverseInertiaTensor = inverseTensor;
}

Vector3f PhysicsComponent::GetVelocity() const {
	std::lock_guard lock(m_mutex.mutex);
	return velocity;
}

Vector3f PhysicsComponent::GetCurrentFrameVelocityDelta() const {
	return currentFrameVelocityDelta;
}

Vector3f PhysicsComponent::GetVelocityOfPoint(const Vector3f& localSpacePoint) const {
	std::lock_guard lock(m_mutex.mutex);
	return velocity + angularVelocity.Cross(localSpacePoint);
}

Vector3f PhysicsComponent::GetAcceleration() const {
	std::lock_guard lock(m_mutex.mutex);
	return acceleration;
}

Vector3f PhysicsComponent::GetAngularVelocity() const {
	std::lock_guard lock(m_mutex.mutex);
	return angularVelocity;
}

void PhysicsComponent::_ResetForces() {
	acceleration = Vector3f::Zero;
}

void PhysicsComponent::_ResetCurrentFrameDeltas() {
	std::lock_guard lock(m_mutex.mutex);
	currentFrameVelocityDelta = Vector3f::Zero;
}

void PhysicsComponent::ApplyForce(const Vector3f& force) {
	// Linear motion.
	_SetAcceleration(acceleration + force * GetInverseMass());
}

void PhysicsComponent::ApplyImpulse(const Vector3f& impulse, const Vector3f& localSpacePoint) {
	Vector3f torque = GetTorque(localSpacePoint, impulse);

	_SetVelocity(velocity + impulse * GetInverseMass());
	_SetAngularVelocity(angularVelocity + Vector3f(torque.ToGlm() * inverseInertiaTensor)); // * GetInverseMass(): no
}

Vector3f PhysicsComponent::GetTorque(const Vector3f& localSpacePoint, const Vector3f& force) {
	return localSpacePoint.Cross(force);
}

void PhysicsComponent::_SetVelocity(Vector3f velocity) {
	std::lock_guard lock(m_mutex.mutex);
	currentFrameVelocityDelta += this->velocity - velocity;
	this->velocity = velocity;
}

void PhysicsComponent::_SetAngularVelocity(Vector3f angularVelocity) {
	std::lock_guard lock(m_mutex.mutex);
	this->angularVelocity = angularVelocity;
}

void PhysicsComponent::_SetAcceleration(Vector3f acceleration) {
	std::lock_guard lock(m_mutex.mutex);
	this->acceleration = acceleration;
}


template <>
nlohmann::json PERSISTENCE::SerializeJson<OSK::ECS::PhysicsComponent>(const OSK::ECS::PhysicsComponent& data) {
	nlohmann::json output{};

	output["mass"] = data.mass;

	output["inertiaTensor"] = SerializeJson<glm::mat3>(data.inertiaTensor);

	output["velocity"]["x"] = data.velocity.x;
	output["velocity"]["y"] = data.velocity.y;
	output["velocity"]["z"] = data.velocity.z;

	output["acceleration"]["x"] = data.acceleration.x;
	output["acceleration"]["y"] = data.acceleration.y;
	output["acceleration"]["z"] = data.acceleration.z;

	output["angularVelocity"]["x"] = data.angularVelocity.x;
	output["angularVelocity"]["y"] = data.angularVelocity.y;
	output["angularVelocity"]["z"] = data.angularVelocity.z;

	return output;
}

template <>
OSK::ECS::PhysicsComponent PERSISTENCE::DeserializeJson<OSK::ECS::PhysicsComponent>(const nlohmann::json& json) {
	PhysicsComponent output{};

	output.mass = json["mass"];

	output.inertiaTensor = DeserializeJson<glm::mat3>(json["inertiaTensor"]);

	output.velocity = Vector3f(
		json["velocity"]["x"],
		json["velocity"]["y"],
		json["velocity"]["z"]
	);

	output.acceleration = Vector3f(
		json["acceleration"]["x"],
		json["acceleration"]["y"],
		json["acceleration"]["z"]
	);

	output.angularVelocity = Vector3f(
		json["angularVelocity"]["x"],
		json["angularVelocity"]["y"],
		json["angularVelocity"]["z"]
	);

	return output;
}
