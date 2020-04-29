#include "KeyboardState.h"

namespace OSK {

	KeyState KeyboardState::GetKeyState(const Key& key) {
		return static_cast<KeyState>(KeyStates[static_cast<keyCode_t>(key)]);
	}


	keyState_t KeyboardState::IsKeyDown(const Key& key) {
		return KeyStates[static_cast<keyCode_t>(key)];
	}


	keyState_t KeyboardState::IsKeyUp(const Key& key) {
		return !IsKeyDown(key);
	}

}