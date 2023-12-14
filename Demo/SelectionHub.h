#pragma once

#include <OSKengine/GameObject.h>

#include "CarSpawner.h"

#include <string>

class SelectionHub {

public:

	void Spawn(CarSpawner* spawner, std::string_view assetsPath);
	void Unspawn();

	void SetCar(CarSpawner* spawner, std::string_view assetsPath);

	OSK::ECS::GameObjectIndex GetCarObject() const;

	std::string_view GetCarAssetsPath() const;

private:

	OSK::ECS::GameObjectIndex m_hubObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex m_centralCarObject = OSK::ECS::EMPTY_GAME_OBJECT;

	std::string m_centralCarAssetsPath = "";

};
