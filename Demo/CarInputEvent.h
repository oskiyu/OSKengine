#pragma once

#include <OSKengine/Event.h>
#include <OSKengine/GameObject.h>

struct CarInputEvent {

	OSK_EVENT("CarInputEvent");

	enum class Type {
		ACCELERATE,
		DECELERATE,
		TURN_LEFT,
		TURN_RIGHT
	};

	Type type;
	OSK::ECS::GameObjectIndex carObject = OSK::ECS::EMPTY_GAME_OBJECT;

};