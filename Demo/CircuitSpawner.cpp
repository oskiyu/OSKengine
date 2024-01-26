#include "CircuitSpawner.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/PhysicsComponent.h>
#include <OSKengine/CollisionComponent.h>

#include <OSKengine/Collider.h>
#include <OSKengine/ConvexVolume.h>
#include <OSKengine/AxisAlignedBoundingBox.h>
#include <OSKengine/PreBuiltCollider.h>
#include <OSKengine/PreBuiltSpline3D.h>

#include <OSKengine/Model3D.h>
#include <OSKengine/ModelLoader3D.h> 

#include <OSKengine/AssetManager.h>

#include "CarComponent.h"
#include "CircuitComponent.h"

OSK::ECS::GameObjectIndex CircuitSpawner::Spawn() {
	SpawnBillboards();
	SpawnTrees();
	SpawnTreesNormals();
	return SpawnCircuit();
}

OSK::ECS::GameObjectIndex CircuitSpawner::SpawnCircuit() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex output = ecse->SpawnObject();

	OSK::ECS::Transform3D			transform(output);
	OSK::ECS::PhysicsComponent		physicsComponent{};
	OSK::ECS::CollisionComponent	collider{};
	OSK::ECS::ModelComponent3D		modelComponent{};
	CircuitComponent				circuitComponent{};

	physicsComponent.SetImmovable();
	physicsComponent.frictionCoefficient = 0.0f;

	const auto& loadedCollider
		= OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::PreBuiltCollider>("Resources/Assets/Collision/circuit_colliders.json");

	collider.SetCollider(loadedCollider);
	// collider.GetCollider()->SetTopLevelCollider(new OSK::COLLISION::AxisAlignedBoundingBox({ 5000.0f, 200.0f, 5000.0f }));
	auto blc = OSK::COLLISION::ConvexVolume::CreateObb({ 2000.0f, 200.0f, 2000.0f });
	blc.AddOffset(OSK::Vector3f(0.0f, -200.0f, 0.0f));

	collider.GetCollider()->AddBottomLevelCollider(blc.CreateCopy());
		
	auto carModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/circuit0.json");

	modelComponent.SetModel(carModel);

	circuitComponent.trazadaIdeal = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::PreBuiltSpline3D>("Resources/Assets/Curves/circuit0.json");

	ecse->AddComponent(output, transform);
	ecse->AddComponent(output, physicsComponent);
	ecse->AddComponent(output, std::move(collider));
	ecse->AddComponent(output, std::move(modelComponent));
	ecse->AddComponent(output, circuitComponent);

	OSK::Engine::GetLogger()->InfoLog(std::format("Spline count: {}", circuitComponent.trazadaIdeal->Get().GetPointCount()));

	return output;
}

void CircuitSpawner::SpawnBillboards() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const OSK::ECS::GameObjectIndex billboards = ecse->SpawnObject();
	

	auto billboardsModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/circuit0_billboards.json");

	OSK::ECS::ModelComponent3D* billboardsModelComponent 
		= &ecse->AddComponent<OSK::ECS::ModelComponent3D>(billboards, {});

	billboardsModelComponent->SetModel(billboardsModel);

	ecse->AddComponent<OSK::ECS::Transform3D>(billboards, OSK::ECS::Transform3D(billboards));
}

void CircuitSpawner::SpawnTreesNormals() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const auto treeNormals = ecse->SpawnObject();

	ecse->AddComponent<OSK::ECS::Transform3D>(treeNormals, OSK::ECS::Transform3D(treeNormals));

	auto treeNormalsModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/circuit0_tree_normals.json");

	OSK::ECS::ModelComponent3D* treeNormalsModelComponent 
		= &ecse->AddComponent<OSK::ECS::ModelComponent3D>(treeNormals, {});

	treeNormalsModelComponent->SetModel(treeNormalsModel); // animModel
	treeNormalsModelComponent->SetCastShadows(false);
}

void CircuitSpawner::SpawnTrees() {
	OSK::ECS::EntityComponentSystem* ecse = OSK::Engine::GetEcs();

	const auto trees = ecse->SpawnObject();

	ecse->AddComponent<OSK::ECS::Transform3D>(trees, OSK::ECS::Transform3D(trees));

	auto treesModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/Models/circuit0_trees.json");

	OSK::ECS::ModelComponent3D* treesModelComponent = &ecse->AddComponent<OSK::ECS::ModelComponent3D>(trees, {});

	treesModelComponent->SetModel(treesModel);
	treesModelComponent->SetCastShadows(false);
}

void CircuitSpawner::SetMaterial3D(OSK::GRAPHICS::Material* material3D) {
	m_material3D = material3D;
}
