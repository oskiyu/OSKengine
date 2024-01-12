#include "CarSpawner.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/PhysicsComponent.h>
#include <OSKengine/CollisionComponent.h>

#include <OSKengine/Collider.h>
#include <OSKengine/ConvexVolume.h>
#include <OSKengine/SphereCollider.h>

#include <OSKengine/Model3D.h>
#include <OSKengine/ModelLoader3D.h> 

#include <OSKengine/AssetManager.h>
#include <OSKengine/AudioAsset.h>
#include "CarAssets.h"

#include "EngineComponent.h"
#include "CarComponent.h"
#include "CarAiComponent.h"
#include "CarControllerComponent.h"

OSK::ECS::GameObjectIndex CarSpawner::Spawn(const OSK::Vector3f& position, std::string_view assetsPath) {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex output = ecse->SpawnObject();

	OSK::ECS::Transform3D			transform(output);
	OSK::ECS::PhysicsComponent		physicsComponent{};
	OSK::ECS::CollisionComponent	collider{};
	OSK::ECS::ModelComponent3D		modelComponent{};
	CarComponent					carComponent{};
	CarAiComponent					carAiComponent{};
	CarControllerComponent			carControllerComponent{};
	EngineComponent					engineComponent{};

	transform.SetPosition(position);

	physicsComponent.SetMass(500.0f);
	// physicsComponent.centerOfMassOffset = OSK::Vector3f(0.0f, 0.17f * 0.5f, 0.0f);
	physicsComponent.coefficientOfRestitution = 0.0f;
	physicsComponent.frictionCoefficient = 0.0f;
	physicsComponent.SetInertiaTensor(glm::mat3(600.0f));

	auto carAssets = OSK::Engine::GetAssetManager()->Load<CarAssets>(static_cast<std::string>(assetsPath));

	modelComponent.SetModel(carAssets->GetModel());

	collider.SetCollider(carAssets->GetCollider());

	const auto sound = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::AudioAsset>("Resources/Assets/Audio/bounce_audio.json");
	
	// carComponent.m_trazada = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::PreLoadedSpline3D>("Resources/Assets/Curves/circuit0.json")->GetCopy();

	// Wheels
	const float deltaX = 0.45f;
	carComponent.wheelBackLeft = SpawnSingleWheel(OSK::Vector3f(deltaX, 0.0f, -0.72f));
	carComponent.wheelBackRight = SpawnSingleWheel(OSK::Vector3f(-deltaX, 0.0f, -0.72f));
	carComponent.wheelFrontLeft = SpawnSingleWheel(OSK::Vector3f(deltaX, 0.0f, 0.6f));
	carComponent.wheelFrontRight = SpawnSingleWheel(OSK::Vector3f(-deltaX, 0.0f, 0.6f));


	ecse->AddComponent(output, transform);
	ecse->AddComponent(output, physicsComponent);
	ecse->AddComponent(output, std::move(collider));
	ecse->AddComponent(output, std::move(modelComponent));
	ecse->AddComponent(output, carComponent);
	ecse->AddComponent(output, std::move(carAiComponent));
	ecse->AddComponent(output, std::move(carControllerComponent));
	auto& motor = ecse->AddComponent(output, std::move(engineComponent));

	motor.audioSource.Init();
	motor.audioSource.SetBuffer(sound->GetBuffer());
	motor.audioSource.SetLooping(true);
	motor.audioSource.SetGain(0.25f);
	// motor.audioSource.Play();

	auto& transformBackLeft = ecse->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelBackLeft);
	auto& transformBackRight = ecse->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelBackRight);
	auto& transformFrontLeft = ecse->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelFrontLeft);
	auto& transformFrontRight = ecse->GetComponent<OSK::ECS::Transform3D>(carComponent.wheelFrontRight);


	transformBackRight.RotateLocalSpace(glm::radians(180.0f), OSK::Vector3f(0.0f, 1.0f, 0.0f));
	transformFrontRight.RotateLocalSpace(glm::radians(180.0f), OSK::Vector3f(0.0f, 1.0f, 0.0f));

	transformBackLeft.AttachToObject(output);
	transformBackRight.AttachToObject(output);
	transformFrontLeft.AttachToObject(output);
	transformFrontRight.AttachToObject(output);


	return output;
}

OSK::ECS::GameObjectIndex CarSpawner::SpawnSingleWheel(const OSK::Vector3f& position) {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex output = ecse->SpawnObject();

	OSK::ECS::Transform3D			transform(output);
	OSK::ECS::ModelComponent3D		modelComponent{};

	transform.SetPosition(position + OSK::Vector3f(0.0f, 0.12f, 0.0f));

	modelComponent.SetModel(
		OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/wheel.json"));

	ecse->AddComponent(output, transform);
	ecse->AddComponent(output, modelComponent);

	return output;
}

void CarSpawner::SetMaterial3D(OSK::GRAPHICS::Material* material3D) {
	m_material3D = material3D;
}
