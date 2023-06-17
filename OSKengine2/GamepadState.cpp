#include "GamepadState.h"

using namespace OSK;
using namespace OSK::IO;

GamepadState::GamepadState(UIndex32 identifier) : identifier(identifier) {
	
}

GamepadButtonState GamepadState::GetButtonState(GamepadButton button) const {
	return buttonState[(UIndex32)button];
}

bool GamepadState::IsButtonDown(GamepadButton button) const {
	return buttonState[(UIndex32)button] == GamepadButtonState::PRESSED;
}

bool GamepadState::IsButtonUp(GamepadButton button) const {
	return buttonState[(UIndex32)button] == GamepadButtonState::RELEASED;
}

float GamepadState::GetAxisState(GamepadAxis axis) const {
	if (axis == GamepadAxis::L2 || axis == GamepadAxis::R2)
		return (axesStates[(UIndex32)axis] + 1.0f) / 2.0f;

	return axesStates[(UIndex32)axis];
}

UIndex32 GamepadState::GetIdentifier() const {
	return identifier;
}

void GamepadState::_SetButtonState(GamepadButton button, GamepadButtonState state) {
	buttonState[(UIndex32)button] = state;
}

void GamepadState::_SetAxisState(GamepadAxis axis, float value) {
	axesStates[(UIndex32)axis] = value;
}

void GamepadState::_SetConnectionState(bool isConnected) {
	this->isConnected = isConnected;
}

bool GamepadState::IsConnected() const {
	return isConnected;
}
