#include "PhysicsSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "TransformComponent3D.h"
#include "PhysicsComponent.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::ECS;

void PhysicsSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<TransformComponent3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<PhysicsComponent>());
	_SetSignature(signature);
}

void PhysicsSystem::Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) {
	for (const GameObjectIndex obj : objects) {
		auto& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(obj).GetTransform();
		auto& physicsComponent = Engine::GetEcs()->GetComponent<PhysicsComponent>(obj);

		if (physicsComponent.IsImmovable())
			continue;

		const auto velocity = physicsComponent.GetVelocity();
		const auto angularVelocity = physicsComponent.GetAngularVelocity();
		const auto acceleration = physicsComponent.GetAcceleration();

		transform.AddPosition(velocity * deltaTime);
		physicsComponent._SetVelocity(velocity + (acceleration + m_gravity) * deltaTime);
		
		if (physicsComponent.GetAngularVelocity().GetLenght() > 0.00001f) {
			transform.RotateWorldSpace(
				physicsComponent.GetAngularVelocity().GetLenght() * deltaTime,
				physicsComponent.GetAngularVelocity().GetNormalized()
			);

			physicsComponent._SetAngularVelocity(angularVelocity * (1.0f - (0.9f * deltaTime)));
		}

		physicsComponent._ResetForces();
		physicsComponent._ResetCurrentFrameDeltas();
	}
}

const Vector3f& PhysicsSystem::GetGravity() const {
	return m_gravity;
}

nlohmann::json PhysicsSystem::SaveConfiguration() const {
	auto output = nlohmann::json();

	output["gravity"]["x"] = m_gravity.x;
	output["gravity"]["y"] = m_gravity.y;
	output["gravity"]["z"] = m_gravity.z;

	return output;
}

PERSISTENCE::BinaryBlock PhysicsSystem::SaveBinaryConfiguration() const {
	auto data = PERSISTENCE::BinaryBlock::Empty();

	data.AppendBlock(PERSISTENCE::SerializeBinaryVector3(m_gravity));

	return data;
}

void PhysicsSystem::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	m_gravity = Vector3f(
		config["gravity"]["x"],
		config["gravity"]["y"],
		config["gravity"]["z"]
	);
}

void PhysicsSystem::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	m_gravity = PERSISTENCE::DeserializeBinaryVector3<Vector3f, float>(reader);
}
