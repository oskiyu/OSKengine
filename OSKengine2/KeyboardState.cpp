#include "KeyboardState.h"

#include <memory>

using namespace OSK::IO;

KeyboardState::KeyboardState() {
	memset(keyStates, 0, sizeof(keyStates));
}

KeyState KeyboardState::GetKeyState(Key key) const {
	return (KeyState)(int)keyStates[(int)key];
}

bool KeyboardState::IsKeyDown(Key key) const {
	return keyStates[(int)key];
}

bool KeyboardState::IsKeyUp(Key key) const {
	return !IsKeyDown(key);
}

void KeyboardState::_SetKeyState(Key key, KeyState state) {
	keyStates[(int)key] = (int)state;
}

void KeyboardState::operator=(const KeyboardState& other) {
	memcpy(keyStates, other.keyStates, sizeof(keyStates));
}
