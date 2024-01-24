// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IKeyboardInput.h"

using namespace OSK;
using namespace OSK::IO;

void IKeyboardInput::UpdateKeyboardInput() {
	oldState = newState;
	UpdateKeyboardState(&newState);
}

bool IKeyboardInput::IsKeyDown(Key key) const { 
	return newState.IsKeyDown(key); 
}

bool IKeyboardInput::IsKeyUp(Key key) const { 
	return newState.IsKeyUp(key); 
}

bool IKeyboardInput::IsKeyStroked(Key key) const {
	return newState.IsKeyDown(key) && oldState.IsKeyUp(key);
}

bool IKeyboardInput::IsKeyReleased(Key key) const {
	return newState.IsKeyUp(key) && oldState.IsKeyDown(key);
}

const KeyboardState& IKeyboardInput::GetKeyboardState() const {
	return newState;
}

const KeyboardState& IKeyboardInput::GetPreviousKeyboardState() const {
	return oldState;
}
