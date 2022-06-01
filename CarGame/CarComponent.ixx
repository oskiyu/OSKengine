module;

#include <OSKengine/Component.h>
#include <OSKengine/DynamicArray.hpp>

export module CarComponent;

export import CarControlMessage;

using namespace OSK;

export struct CarComponent {

public:

	OSK_COMPONENT("CarComponent");

	float accelPercent = 0.0f;
	float anglePercent = 0.0f;

	float angle = 0.00f;
	float speed = 0.0f;

	DynamicArray<CarControlMessage> controller;

	inline void Accelerate(float amount) {
		controller.Insert(CarControlMessage(CarControlMessage::Type::ACCEL, amount));
	}

	inline void Turn(float amount) {
		controller.Insert(CarControlMessage(CarControlMessage::Type::TURN, amount));
	}

	inline void Stop() {
		controller.Insert(CarControlMessage(CarControlMessage::Type::STOP, 0.0f));
	}

};
