#pragma once

#include <OSKengine/Component.h>
#include <OSKengine/GameObject.h>

struct CarComponent {

	OSK_COMPONENT("CarComponent");

	OSK::ECS::GameObjectIndex wheelFrontLeft = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex wheelFrontRight = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex wheelBackLeft = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex wheelBackRight = OSK::ECS::EMPTY_GAME_OBJECT;

	float previousFrameTurning = 0.0f;

	float maxTurning = 30.0f;
	float maxTurningPerSecond = 80.0f;

	float GetMaxSpeedForCorner(float degrees) const {
		if (degrees < 10.0f) {
			return 210.0f / 3.6f;
		}
		else if (degrees < 40.0f) {
			return 130.0f / 3.6f;
		}
		else if (degrees < 60.0f) {
			return 90.0f / 3.6f;
		}
		else if (degrees < 90.0f) {
			return 50.0f / 3.6f;
		}
		else {
			return 1.0f / 3.6f;
		}
	}

};
