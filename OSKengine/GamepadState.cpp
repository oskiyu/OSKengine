#include "GamepadState.h"

using namespace OSK;

buttonState_t GamepadState::GetButtonState(GamepadButton button) const {
	return (buttonState_t)buttonStates[(int)button];
}

bool GamepadState::IsButtonDown(GamepadButton button) const {
	return GetButtonState(button);
}

bool GamepadState::IsButtonUp(GamepadButton button) const {
	return !GetButtonState(button);
}

float GamepadState::GetAxisState(GamepadAxis axis) const {
	if (axis == GamepadAxis::L2 || axis == GamepadAxis::R2)
		return (axesStates[(int)axis] + 1.0f) / 2.0f;

	return axesStates[(int)axis];
}

gamepadId_t GamepadState::GetID() const {
	return identifier;
}

bool GamepadState::IsConnected() const {
	return isConnected;
}
