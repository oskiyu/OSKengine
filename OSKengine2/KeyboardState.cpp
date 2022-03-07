#include "KeyboardState.h"

using namespace OSK::IO;

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
