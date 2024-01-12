#pragma once

#include <OSKengine/OSKmacros.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/Material.h>
#include <OSKengine/Vector3.hpp>

class CarSpawner {

public:

	OSK::ECS::GameObjectIndex Spawn(const OSK::Vector3f& position, std::string_view assetsPath);

	void SetMaterial3D(OSK::GRAPHICS::Material* material3D);

private:

	OSK::ECS::GameObjectIndex SpawnSingleWheel(const OSK::Vector3f& position);

	OSK::GRAPHICS::Material* m_material3D = nullptr;

};
