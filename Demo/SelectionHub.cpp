#include "SelectionHub.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/PhysicsComponent.h>
#include <OSKengine/CollisionComponent.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/AxisAlignedBoundingBox.h>

void SelectionHub::Spawn(CarSpawner* spawner, std::string_view assetsPath) {
	SetCar(spawner, assetsPath);

	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	m_hubObject = ecse->SpawnObject();

	OSK::ECS::Transform3D			transform(m_hubObject);
	OSK::ECS::PhysicsComponent		physicsComponent{};
	OSK::ECS::CollisionComponent	collider{};
	OSK::ECS::ModelComponent3D		modelComponent{};

	physicsComponent.SetImmovable();

	const auto& loadedCollider
		= OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::PreBuiltCollider>("Resources/Assets/Collision/circuit_colliders.json");

	collider.SetCollider(loadedCollider);
	collider.GetCollider()->SetTopLevelCollider(new OSK::COLLISION::AxisAlignedBoundingBox({ 100.0f, 5.0f, 100.0f }));


	auto carModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/HUB/white.json");

	modelComponent.SetModel(carModel);

	ecse->AddComponent(m_hubObject, transform);
	ecse->AddComponent(m_hubObject, physicsComponent);
	ecse->AddComponent(m_hubObject, std::move(collider));
	ecse->AddComponent(m_hubObject, std::move(modelComponent));
}

void SelectionHub::Unspawn() {
	auto* ecs = OSK::Engine::GetEcs();

	if (m_hubObject != OSK::ECS::EMPTY_GAME_OBJECT) {
		ecs->DestroyObject(&m_hubObject);
	}

	if (m_centralCarObject != OSK::ECS::EMPTY_GAME_OBJECT) {
		ecs->DestroyObject(&m_centralCarObject);
	}
}

void SelectionHub::SetCar(CarSpawner* spawner, std::string_view assetsPath) {
	if (m_centralCarObject != OSK::ECS::EMPTY_GAME_OBJECT) {
		OSK::Engine::GetEcs()->DestroyObject(&m_centralCarObject);
	}

	m_centralCarObject = spawner->Spawn(OSK::Vector3f::Zero, assetsPath);

	m_centralCarAssetsPath = static_cast<std::string>(assetsPath);
}

OSK::ECS::GameObjectIndex SelectionHub::GetCarObject() const {
	return m_centralCarObject;
}

std::string_view SelectionHub::GetCarAssetsPath() const {
	return m_centralCarAssetsPath;
}
