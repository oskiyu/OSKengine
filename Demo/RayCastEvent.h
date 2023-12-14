#pragma once

#include <OSKengine/Event.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/RayCastResult.h>

#include <string>


struct RayCastEvent {

	OSK_EVENT("RayCastEvent");

	OSK::ECS::GameObjectIndex car = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::COLLISION::RayCastResult rayResult = OSK::COLLISION::RayCastResult::False();

};
