#pragma once

#include <OSKengine/OSKmacros.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/Material.h>

class CarSpawner {

public:

	OSK::ECS::GameObjectIndex Spawn();

	void SetMaterial3D(OSK::GRAPHICS::Material* material3D);

private:

	OSK::GRAPHICS::Material* material3D = nullptr;

};
