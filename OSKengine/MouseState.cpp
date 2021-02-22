#include "MouseState.h"

namespace OSK {

	ButtonState MouseState::GetButtonState(ButtonCode button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return static_cast<ButtonState>(ButtonStates[0]);

		if (button == ButtonCode::BUTTON_RIGHT)
			return static_cast<ButtonState>(ButtonStates[1]);

		if (button == ButtonCode::BUTTON_MIDDLE)
			return static_cast<ButtonState>(ButtonStates[2]);

		return static_cast<ButtonState>(ButtonStates[static_cast<buttonCode_t>(button)]);
	}


	bool MouseState::IsButtonDown(ButtonCode button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return ButtonStates[0];

		if (button == ButtonCode::BUTTON_RIGHT)
			return ButtonStates[1];

		if (button == ButtonCode::BUTTON_MIDDLE)
			return ButtonStates[2];

		return ButtonStates[static_cast<buttonCode_t>(button)];
	}


	bool MouseState::IsButtonUp(ButtonCode button) const {
		return !IsButtonDown(button);
	}


	Vector4 MouseState::GetMouseRectangle(float size) const {
		return Vector4(PositionX, PositionY, size, size);
	}

}