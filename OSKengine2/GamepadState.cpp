#include "GamepadState.h"

using namespace OSK;
using namespace OSK::IO;

GamepadState::GamepadState(TSize identifier) : identifier(identifier) {
	
}

GamepadButtonState GamepadState::GetButtonState(GamepadButton button) const {
	return buttonState[(TSize)button];
}

bool GamepadState::IsButtonDown(GamepadButton button) const {
	return buttonState[(TSize)button] == GamepadButtonState::PRESSED;
}

bool GamepadState::IsButtonUp(GamepadButton button) const {
	return buttonState[(TSize)button] == GamepadButtonState::RELEASED;
}

float GamepadState::GetAxisState(GamepadAxis axis) const {
	if (axis == GamepadAxis::L2 || axis == GamepadAxis::R2)
		return (axesStates[(TSize)axis] + 1.0f) / 2.0f;

	return axesStates[(TSize)axis];
}

TSize GamepadState::GetIdentifier() const {
	return identifier;
}

void GamepadState::_SetButtonState(GamepadButton button, GamepadButtonState state) {
	buttonState[(TSize)button] = state;
}

void GamepadState::_SetAxisState(GamepadAxis axis, float value) {
	axesStates[(TSize)axis] = value;
}

void GamepadState::_SetConnectionState(bool isConnected) {
	this->isConnected = isConnected;
}

bool GamepadState::IsConnected() const {
	return isConnected;
}
