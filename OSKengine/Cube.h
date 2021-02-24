#pragma once

#include "GameObject.h"
#include "Model.h"
#include "PhysicsEntity.h"
#include "ModelComponent.h"

class Cube : public OSK::GameObject {

public:

	void OnCreate() override {
		AddComponent<OSK::PhysicsComponent>({});

		OSK::PhysicsComponent& physics = GetComponent<OSK::PhysicsComponent>();
		
		physics.Collision.ColliderTransform.AttachTo(&Transform3D);

		auto box = OSK::Collision::SAT_Collider::CreateOBB();
		physics.Collision.SatColliders.push_back(box);
		physics.Collision.SatColliders.back().BoxTransform.AttachTo(&Transform3D);
		physics.Collision.BroadType = OSK::BroadColliderType::BOX_AABB;
		physics.Collision.BroadCollider.Box.Size = { 5.0f };

		physics.Height = -1.0f;

		AddComponent<OSK::ModelComponent>({});
	}

};

class PlayerCube : public Cube {

public:

	void OnCreate() override {
		Cube::OnCreate();

		OSK::InputComponent input;

		input.GetInputFunction("MoveFront") = [this](deltaTime_t deltaTime) {
			Transform3D.AddPosition(OSK::Vector3f(3, 0, 0) * deltaTime);
		};
		input.GetInputFunction("MoveBack") = [this](deltaTime_t deltaTime) {
			Transform3D.AddPosition(OSK::Vector3f(-3, 0, 0) * deltaTime);
		};
		input.GetInputFunction("MoveLeft") = [this](deltaTime_t deltaTime) {
			Transform3D.AddPosition(OSK::Vector3f(0, 0, -3) * deltaTime);
		};
		input.GetInputFunction("MoveRight") = [this](deltaTime_t deltaTime) {
			Transform3D.AddPosition(OSK::Vector3f(0, 0, 3) * deltaTime);
		};

		AddComponent<OSK::InputComponent>(input);
	}

};