#include "PhysicsSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "PhysicsComponent.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::ECS;

void PhysicsSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<PhysicsComponent>());
	_SetSignature(signature);
}

void PhysicsSystem::Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) {
	for (const GameObjectIndex obj : objects) {
		auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		auto& physicsComponent = Engine::GetEcs()->GetComponent<PhysicsComponent>(obj);

		if (physicsComponent.IsImmovable())
			continue;

		const auto velocity = physicsComponent.GetVelocity();
		const auto angularVelocity = physicsComponent.GetAngularVelocity();
		const auto acceleration = physicsComponent.GetAcceleration();

		transform.AddPosition(velocity * deltaTime);
		physicsComponent._SetVelocity(velocity + (acceleration + Vector3f(0, -2.6f, 0)) * deltaTime);
		
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

nlohmann::json PhysicsSystem::SaveConfiguration() const {
	auto output = nlohmann::json();

	output["gravityAccel"]["x"] = gravityAccel.x;
	output["gravityAccel"]["y"] = gravityAccel.y;
	output["gravityAccel"]["z"] = gravityAccel.z;

	output["gravity"]["x"] = gravity.x;
	output["gravity"]["y"] = gravity.y;
	output["gravity"]["z"] = gravity.z;

	return output;
}

PERSISTENCE::BinaryBlock PhysicsSystem::SaveBinaryConfiguration() const {
	auto data = PERSISTENCE::BinaryBlock::Empty();

	data.AppendBlock(PERSISTENCE::SerializeBinaryVector3(gravityAccel));
	data.AppendBlock(PERSISTENCE::SerializeBinaryVector3(gravity));

	return data;
}

void PhysicsSystem::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	gravityAccel = Vector3f(
		config["gravityAccel"]["x"],
		config["gravityAccel"]["y"],
		config["gravityAccel"]["z"]
	);

	gravity = Vector3f(
		config["gravity"]["x"],
		config["gravity"]["y"],
		config["gravity"]["z"]
	);
}

void PhysicsSystem::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	gravityAccel = PERSISTENCE::DeserializeBinaryVector3<Vector3f, float>(reader);
	gravity = PERSISTENCE::DeserializeBinaryVector3<Vector3f, float>(reader);
}
