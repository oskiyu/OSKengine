#include "CarSpawner.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/Collider.h>
#include <OSKengine/ConvexVolume.h>
#include <OSKengine/SphereCollider.h>

#include <OSKengine/Model3D.h>
#include <OSKengine/ModelLoader3D.h> 

#include <OSKengine/AssetManager.h>

#include "CarComponent.h"

OSK::ECS::GameObjectIndex CarSpawner::Spawn() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex output = ecse->SpawnObject();

	OSK::ECS::Transform3D		transform(output);
	OSK::ECS::PhysicsComponent	physicsComponent{};
	OSK::COLLISION::Collider	collider{};
	OSK::ECS::ModelComponent3D	modelComponent{};
	CarComponent				carComponent{};

	transform.AddPosition({ 0.0f, 5.0f, 0.1f });

	physicsComponent.SetMass(4.0f);
	physicsComponent.centerOfMassOffset = OSK::Vector3f(0.0f, 0.17f * 0.5f, 0.0f);
	physicsComponent.coefficientOfRestitution = 0.2f;

	OSK::OwnedPtr<OSK::COLLISION::ConvexVolume> convexVolume 
		= new OSK::COLLISION::ConvexVolume(
			OSK::COLLISION::ConvexVolume::CreateObb({ 0.15f * 2, 0.17f, 0.35f * 2 }, 0));

	collider.SetTopLevelCollider(new OSK::COLLISION::SphereCollider(0.45f));
	collider.AddBottomLevelCollider(convexVolume.GetPointer());

	OSK::ASSETS::Model3D* carModel 
		= OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/mclaren.json", "GLOBAL");
	
	modelComponent.SetModel(carModel);
	modelComponent.SetMaterial(m_material3D);
	OSK::ASSETS::ModelLoader3D::SetupPbrModel(*carModel, &modelComponent);

	ecse->AddComponent(output, transform);
	ecse->AddComponent(output, physicsComponent);
	ecse->AddComponent(output, std::move(collider));
	ecse->AddComponent(output, std::move(modelComponent));
	ecse->AddComponent(output, carComponent);

	return output;
}

void CarSpawner::SetMaterial3D(OSK::GRAPHICS::Material* material3D) {
	m_material3D = material3D;
}
