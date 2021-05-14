#include "MouseState.h"

namespace OSK {

	ButtonState MouseState::GetButtonState(ButtonCode button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return static_cast<ButtonState>(buttonStates[0]);

		if (button == ButtonCode::BUTTON_RIGHT)
			return static_cast<ButtonState>(buttonStates[1]);

		if (button == ButtonCode::BUTTON_MIDDLE)
			return static_cast<ButtonState>(buttonStates[2]);

		return static_cast<ButtonState>(buttonStates[static_cast<buttonCode_t>(button)]);
	}

	bool MouseState::IsButtonDown(ButtonCode button) const {
		if (button == ButtonCode::BUTTON_LEFT)
			return buttonStates[0];

		if (button == ButtonCode::BUTTON_RIGHT)
			return buttonStates[1];

		if (button == ButtonCode::BUTTON_MIDDLE)
			return buttonStates[2];

		return buttonStates[static_cast<buttonCode_t>(button)];
	}

	bool MouseState::IsButtonUp(ButtonCode button) const {
		return !IsButtonDown(button);
	}

	Vector4 MouseState::GetMouseRectangle(float size) const {
		return Vector4(positionX, positionY, size, size);
	}

	Vector2f MouseState::GetPosition() const {
		return { positionX, positionY };
	}

	Vector2f MouseState::GetRelativePosition() const {
		return { relativePositionX, relativePositionY };
	}

	float MouseState::GetScrollX() const {
		return scrollX;
	}

	float MouseState::GetScrollY() const {
		return scrollY;
	}

	float MouseState::GetScrollDeltaX() const {
		return scrollX - oldScrollX;
	}

	float MouseState::GetScrollDeltaY() const {
		return scrollY - oldScrollY;
	}

}