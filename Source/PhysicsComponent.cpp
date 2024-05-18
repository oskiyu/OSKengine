#include "PhysicsComponent.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

#include "Math.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::PERSISTENCE;


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
	_SetAngularVelocity(GetAngularVelocity() + Vector3f(torque.ToGlm() * inverseInertiaTensor)); // * GetInverseMass(): no
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
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::PhysicsComponent>(const OSK::ECS::PhysicsComponent& data) {
	nlohmann::json output{};

	if (data.IsImmovable()) {
		output["inverse_mass"] = data.inverseMass;
	}
	else {
		output["mass"] = data.mass;
	}

	output["inertiaTensor"] = SerializeData<glm::mat3>(data.inertiaTensor);

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
OSK::ECS::PhysicsComponent PERSISTENCE::DeserializeComponent<OSK::ECS::PhysicsComponent>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	PhysicsComponent output{};

	if (json.contains("mass")) {
		output.SetMass(json["mass"]);
	}
	else if (json.contains("inverse_mass")) {
		output.SetInverseMass(json["inverse_mass"]);
	}

	output.SetInertiaTensor(DeserializeData<glm::mat3>(json["inertiaTensor"]));

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



template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::PhysicsComponent>(const OSK::ECS::PhysicsComponent& data) {
	BinaryBlock output{};

	output.Write<TByte>(data.IsImmovable());

	if (data.IsImmovable()) {
		output.Write<float>(data.inverseMass);
	}
	else {
		output.Write<float>(data.mass);
	}

	output.AppendBlock(BinarySerializeData<glm::mat3>(data.inertiaTensor));

	output.AppendBlock(SerializeBinaryVector3(data.velocity));
	output.AppendBlock(SerializeBinaryVector3(data.acceleration));
	output.AppendBlock(SerializeBinaryVector3(data.angularVelocity));

	return output;
}

template <>
OSK::ECS::PhysicsComponent PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::PhysicsComponent>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	PhysicsComponent output{};

	const bool isInmovable = reader->Read<TByte>();

	if (isInmovable) {
		output.SetInverseMass(reader->Read<float>());
	}
	else {
		output.SetMass(reader->Read<float>());
	}

	output.SetInertiaTensor(BinaryDeserializeData<glm::mat3>(reader));

	output.velocity = DeserializeBinaryVector3<Vector3f, float>(reader);
	output.acceleration = DeserializeBinaryVector3<Vector3f, float>(reader);
	output.angularVelocity = DeserializeBinaryVector3<Vector3f, float>(reader);

	return output;
}
