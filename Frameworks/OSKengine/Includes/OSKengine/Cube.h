#pragma once

#include "GameObject.h"
#include "Model.h"
#include "PhysicsEntity.h"
#include "ModelComponent.h"

class Cube : public OSK::GameObject {

public:

	OSK_GAME_OBJECT(Cube)

	void OnCreate() override {
		AddComponent<OSK::PhysicsComponent>(OSK::PhysicsComponent());

		OSK::PhysicsComponent& physics = GetComponent<OSK::PhysicsComponent>();
		
		physics.GetCollider().GetTransform().AttachTo(this->GetTransform());

		auto box = OSK::Collision::SAT_Collider::CreateOBB();
		physics.GetCollider().satColliders.push_back(box);
		physics.GetCollider().satColliders.back().GetTransform().AttachTo(this->GetTransform());
		physics.GetCollider().SetBroadCollider(OSK::CollisionBox({ 0.0f }, { 5.0f }));

		physics.gravityCenterHeight = -2.0f;

		AddComponent<OSK::ModelComponent>({});
	}

};

class PlayerCube : public Cube {

public:

	OSK_GAME_OBJECT(PlayerCube)

	void OnCreate() override {
		Cube::OnCreate();

		OSK::InputComponent input;

		input.GetInputFunction("MoveFront") = [this](deltaTime_t deltaTime) {
			GetTransform()->AddPosition(OSK::Vector3f(3, 0, 0) * deltaTime);
			OSK::Logger::DebugLog("XD)");
		};
		input.GetInputFunction("MoveBack") = [this](deltaTime_t deltaTime) {
			GetTransform()->AddPosition(OSK::Vector3f(-3, 0, 0) * deltaTime);
		};
		input.GetInputFunction("MoveLeft") = [this](deltaTime_t deltaTime) {
			GetTransform()->AddPosition(OSK::Vector3f(0, 0, -3) * deltaTime);
		};
		input.GetInputFunction("MoveRight") = [this](deltaTime_t deltaTime) {
			GetTransform()->AddPosition(OSK::Vector3f(0, 0, 3) * deltaTime);
		};

		AddComponent<OSK::InputComponent>(input);
	}

};
