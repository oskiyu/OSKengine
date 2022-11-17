// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IMouseInput.h"

#include "OSKengine.h"

using namespace OSK;
using namespace OSK::IO;

void IMouseInput::UpdateMouseInput() {
	oldState = newState;
	UpdateMouseState(&newState);
}

const MouseState& IMouseInput::GetMouseState() const {
	return newState;
}

const MouseState& IMouseInput::GetPreviousMouseState() const {
	return oldState;
}
