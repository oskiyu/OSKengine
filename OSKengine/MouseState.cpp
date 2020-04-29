#include "MouseState.h"

namespace OSK {

	ButtonState MouseState::GetButtonState(const ButtonCode& button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return static_cast<ButtonState>(ButtonStates[0]);

		if (button == ButtonCode::BUTTON_RIGHT)
			return static_cast<ButtonState>(ButtonStates[1]);

		if (button == ButtonCode::BUTTON_MIDDLE)
			return static_cast<ButtonState>(ButtonStates[2]);

		return static_cast<ButtonState>(ButtonStates[static_cast<buttonCode_t>(button)]);
	}


	bool MouseState::IsButtonDown(const ButtonCode& button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return ButtonStates[0];

		if (button == ButtonCode::BUTTON_RIGHT)
			return ButtonStates[1];

		if (button == ButtonCode::BUTTON_MIDDLE)
			return ButtonStates[2];

		return ButtonStates[static_cast<buttonCode_t>(button)];
	}


	bool MouseState::IsButtonUp(const ButtonCode& button) const {
		return !IsButtonDown(button);
	}


	Vector4 MouseState::GetMouseRectangle(const float_t& size) const {
		return Vector4(PositionX, PositionY, size, size);
	}

}