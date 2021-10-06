#pragma once

#include "GameObject.h"
#include "Model.h"
#include "PhysicsEntity.h"
#include "ModelComponent.h"
#include "OnTickComponent.h"
#include "InputComponent.h"

#include "Math.h"
#include "Units.h"
#include "FileIO.h"

#include "PropertiesFile.h"

#define OSK_MS_TO_KMH(x) x * 3600 / 1000
#define OSK_KMH_TO_MS(x) x * 1000 / 3600

class Wheel : public OSK::GameObject {

public:

	OSK_GAME_OBJECT(Wheel);

	void OnCreate() override {
		AddComponent<OSK::ModelComponent>({});
	}

	float angle = 0.0f;

};

class Car : public OSK::GameObject {

public:

	OSK_GAME_OBJECT(Car);

	void OnCreate() override {
		AddComponent<OSK::PhysicsComponent>(OSK::PhysicsComponent());

		OSK::PhysicsComponent& physics = GetComponent<OSK::PhysicsComponent>();

		physics.GetCollider().GetTransform()->AttachTo(this->GetTransform());

		physics.gravityCenterHeight = 0.0f;
		physics.SetMobility(OSK::PhysicalEntityMobilityType::MOVABLE);
		
		AddComponent<OSK::ModelComponent>({});

		SetupInput();

		OSK::OnTickComponent onTick;
		onTick.OnTick = [this](deltaTime_t deltaTime) {
			OnTick(deltaTime);
		};

		AddComponent<OSK::OnTickComponent>(onTick);

		Reload();
	}

	void SetSound(OSK::SoundEmitterComponent* sound) {
		this->sound = sound;

		sound->SetGain(0.1f);
	}

	void Reload() {
		OSK::PropertiesFile carProp;
		carProp.ReadFile("Game/Data/Cars/car.prop");

		maxSpeed = carProp.GetFloat("MAX_SPEED");
		maxAcceleration = carProp.GetFloat("ACCEL");
	}

	Wheel* fl = nullptr;
	Wheel* fr = nullptr;
	Wheel* bl = nullptr;
	Wheel* br = nullptr;

private:

	void SetupInput() {
		OSK::InputComponent input;

		float deccelSensitivity = 12.0f;
		float sensitivity = maxAngle;

		input.GetAxisInputFunction("Acelerar") = [this](deltaTime_t deltaTime, float diff) {
			acceleration += maxAcceleration * diff;
		};
		input.GetAxisInputFunction("Frenar") = [this, deccelSensitivity](deltaTime_t deltaTime, float diff) {
			acceleration += -deccelSensitivity * diff;
		};
		input.GetAxisInputFunction("GirarL") = [this, sensitivity](deltaTime_t deltaTime, float diff) {
			nextAngle += sensitivity * -diff;
		};

		AddComponent<OSK::InputComponent>(input);
	}

	void OnTick(deltaTime_t deltaTime) {
		auto& physicsComponent = GetComponent<OSK::PhysicsComponent>();

		OSK::Vector3f aero = -physicsComponent.velocity * 0.35f;
		OSK::Vector3f motorBrake = -physicsComponent.velocity * 0.4f;

		OSK::Vector3f lateralVel = GetRightVector() * physicsComponent.velocity.Dot(GetRightVector());
		OSK::Vector3f lateralAero = -lateralVel * 1.5f;

		if (glm::abs(nextAngle) > 0.1f) {
			float min = 1.0f;
			if (nextAngle < 0.0f)
				min = -1.0f;

			float radius = 0.9f / glm::sin(glm::radians(OSK::Math::LinearInterpolation(nextAngle, min, physicsComponent.velocity.GetLenght() / OSK_MS_TO_KMH(160))));
			GetTransform()->RotateWorldSpace(physicsComponent.velocity.GetLenght() / radius * deltaTime, { 0, 1, 0 });

			nextAngle = glm::radians(OSK::Math::LinearInterpolation(nextAngle, nextAngle * 0.5f, physicsComponent.velocity.GetLenght() / OSK_MS_TO_KMH(160)));
		}

		physicsComponent.velocity += ((GetForwardVector() * acceleration) + aero + lateralAero) * deltaTime;
		if (acceleration < 0.01f)
			physicsComponent.velocity += motorBrake * deltaTime;

		if (physicsComponent.velocity.GetLenght() > maxSpeed)
			physicsComponent.velocity = physicsComponent.velocity.GetNormalized() * maxSpeed;

		float mult = 5.0f;
		physicsComponent.velocity = GetForwardVector() * physicsComponent.velocity.GetLenght();

		{
			float diff = -(fr->angle - nextAngle);

			fr->GetTransform()->RotateWorldSpace(diff, { 0, mult, 0 });

			fr->angle = nextAngle;
		}
		{
			float diff = -(fl->angle - nextAngle);

			fl->GetTransform()->RotateWorldSpace(diff, { 0, mult, 0 });

			fl->angle = nextAngle;
		}

		nextAngle = 0.0f;

		if (sound) {
			float accelSound = (acceleration / maxAcceleration) * 12 + 1.0f;
			if (accelSound > previousPitch) {
				previousPitch += 2.0f * deltaTime;

				if (previousPitch > accelSound)
					previousPitch = accelSound;
			}
			else if (accelSound < previousPitch)
				previousPitch -= 1.5f * deltaTime;

			sound->SetPitch(previousPitch);
		}

		acceleration = 0;
	}

	float acceleration = 0.0f;
	float maxSpeed = 50; //70 = 252km/h //85 = 306km/h
	float maxAcceleration = 10;

	float previousPitch = 1.0f;

	float nextAngle = 0.0f;
	const float maxAngle = 8;
	float distance = 1.5f;

	OSK::SoundEmitterComponent* sound = nullptr;

};


class Cube : public OSK::GameObject {

public:

	OSK_GAME_OBJECT(Cube)

	void OnCreate() override {
		AddComponent<OSK::PhysicsComponent>(OSK::PhysicsComponent());

		OSK::PhysicsComponent& physics = GetComponent<OSK::PhysicsComponent>();
		
		physics.GetCollider().GetTransform()->AttachTo(this->GetTransform());

		auto box = OSK::Collision::SAT_Collider::CreateOBB();
		physics.GetCollider().satColliders.push_back(box);
		physics.GetCollider().satColliders.back().GetTransform()->AttachTo(this->GetTransform());
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
