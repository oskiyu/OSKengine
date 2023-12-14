#pragma once

#include <OSKengine/Component.h>
#include <OSKengine/GameObject.h>

struct CarControllerComponent {

	OSK_COMPONENT("CarInputEvent");

	float rightTurn = 0.0f; // [-1.0, 1.0]
	float acceleration = 0.0f;

	void Reset() {
		rightTurn = 0.0f;
		acceleration = 0.0f;
	}

};
