module;

#include <OSKengine/KeyboardState.h>
#include <OSKengine/MouseState.h>
#include <OSKengine/GamepadState.h>
#include <OSKengine/GameObject.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/IInputListener.h>
#include <OSKengine/EntityComponentSystem.h>

export module CarInputListener;

import CarComponent;

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;

export class CarInputListener : public IInputListener {

public:

	CarInputListener() {
		SetMinAxisValue(0.25f);
	}

	void OnKeyBeingPressed(Key key) {
		if (controlledObject == EMPTY_GAME_OBJECT)
			return;

		CarComponent& carComponent = Engine::GetEntityComponentSystem()->GetComponent<CarComponent>(controlledObject);

		switch (key) {

		case Key::UP:
			carComponent.Accelerate(1.0f);
			break;
		case Key::DOWN:
			carComponent.Accelerate(-1.0f);
			break;

		case Key::LEFT:
			carComponent.Turn(-1.0f);
			break;
		case Key::RIGHT:
			carComponent.Turn(1.0f);
			break;
		case Key::SPACE:

			carComponent.Stop();
			break;

		}
	}

	void OnGamepadButtonStroke(GamepadButton button, TSize gamepadId) override {
		Engine::Close();
	}

	void OnGamepadAxisValue(GamepadAxis axis, float value, TSize gamepadId) override {
		if (controlledObject == EMPTY_GAME_OBJECT)
			return;

		CarComponent& carComponent = Engine::GetEntityComponentSystem()->GetComponent<CarComponent>(controlledObject);

		switch (axis) {
		case OSK::IO::GamepadAxis::LEFT_X:
			carComponent.Turn(value);
			break;
		case OSK::IO::GamepadAxis::L2:
			carComponent.Accelerate(-value);
			break;
		case OSK::IO::GamepadAxis::R2:
			carComponent.Accelerate(value);
			break;
		}
	}

	inline void SetControlledCar(GameObjectIndex carObject) {
		controlledObject = carObject;
	}

private:

	GameObjectIndex controlledObject = EMPTY_GAME_OBJECT;

};
