#pragma once

#include <OSKengine/OSKmacros.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/Material.h>

class CircuitSpawner {

public:

	OSK::ECS::GameObjectIndex Spawn();

	void SetMaterial3D(OSK::GRAPHICS::Material* material3D);

private:

	OSK::ECS::GameObjectIndex SpawnCircuit();
	void SpawnBillboards();
	void SpawnTrees();
	void SpawnTreesNormals();

	OSK::GRAPHICS::Material* m_material3D = nullptr;

};
