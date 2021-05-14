#include "KeyboardState.h"

namespace OSK {

	KeyState KeyboardState::GetKeyState(Key key) {
		return static_cast<KeyState>(keyStates[static_cast<keyCode_t>(key)]);
	}


	keyState_t KeyboardState::IsKeyDown(Key key) {
		return keyStates[static_cast<keyCode_t>(key)];
	}


	keyState_t KeyboardState::IsKeyUp(Key key) {
		return !IsKeyDown(key);
	}

}