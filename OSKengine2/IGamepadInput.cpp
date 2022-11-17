// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IGamepadInput.h"

using namespace OSK;
using namespace OSK::IO;

IGamepadInput::IGamepadInput() {
	for (TIndex i = 0; i < MAX_GAMEPAD_COUNT; i++) {
		oldStates[i] = GamepadState(i);
		newStates[i] = GamepadState(i);
	}
}

void IGamepadInput::UpdateGamepadInput() {
	for (TIndex i = 0; i < MAX_GAMEPAD_COUNT; i++) {
		oldStates[i] = newStates[i];
		UpdateGamepadState(&newStates[i]);
	}
}
const GamepadState& IGamepadInput::GetGamepadState(TIndex index) const {
	return newStates[index];
}

const GamepadState& IGamepadInput::GetPreviousGamepadState(TIndex index) const {
	return oldStates[index];
}