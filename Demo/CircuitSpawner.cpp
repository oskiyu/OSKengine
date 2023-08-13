#include "CircuitSpawner.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/Collider.h>
#include <OSKengine/ConvexVolume.h>
#include <OSKengine/AxisAlignedBoundingBox.h>

#include <OSKengine/Model3D.h>
#include <OSKengine/ModelLoader3D.h> 

#include <OSKengine/AssetManager.h>

#include "CarComponent.h"

OSK::ECS::GameObjectIndex CircuitSpawner::Spawn() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex output = ecse->SpawnObject();

	OSK::ECS::Transform3D		transform(output);
	OSK::ECS::PhysicsComponent	physicsComponent{};
	OSK::COLLISION::Collider	collider{};
	OSK::ECS::ModelComponent3D	modelComponent{};

	physicsComponent.SetImmovable();

	OSK::OwnedPtr<OSK::COLLISION::ConvexVolume> convexVolume
		= new OSK::COLLISION::ConvexVolume(OSK::COLLISION::ConvexVolume::CreateObb({ 100.0f, 100.0f, 100.0f }, -100.0f));

	collider.SetTopLevelCollider(new OSK::COLLISION::AxisAlignedBoundingBox({ 100.0f, 5.0f, 100.0f }));
	collider.AddBottomLevelCollider(convexVolume.GetPointer());

	OSK::ASSETS::Model3D* carModel
		= OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/circuit0.json", "GLOBAL");

	modelComponent.SetModel(carModel);
	modelComponent.SetMaterial(material3D);
	OSK::ASSETS::ModelLoader3D::SetupPbrModel(*carModel, &modelComponent);

	ecse->AddComponent(output, transform);
	ecse->AddComponent(output, physicsComponent);
	ecse->AddComponent(output, std::move(collider));
	ecse->AddComponent(output, std::move(modelComponent));

	return output;
}

void CircuitSpawner::SetMaterial3D(OSK::GRAPHICS::Material* material3D) {
	this->material3D = material3D;
}
