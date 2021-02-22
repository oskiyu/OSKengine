#include "KeyboardState.h"

namespace OSK {

	KeyState KeyboardState::GetKeyState(Key key) {
		return static_cast<KeyState>(KeyStates[static_cast<keyCode_t>(key)]);
	}


	keyState_t KeyboardState::IsKeyDown(Key key) {
		return KeyStates[static_cast<keyCode_t>(key)];
	}


	keyState_t KeyboardState::IsKeyUp(Key key) {
		return !IsKeyDown(key);
	}

}