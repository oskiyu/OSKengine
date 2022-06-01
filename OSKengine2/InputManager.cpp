#include "InputManager.h"

#include "IInputListener.h"
#include "KeyboardState.h"
#include "MouseState.h"
#include "GamepadState.h"
#include "Window.h"

using namespace OSK;
using namespace OSK::IO;


// Gamepad buttons
const static GamepadButton gamepadButtons[] = {
	GamepadButton::X,
	GamepadButton::Y,
	GamepadButton::A,
	GamepadButton::B,

	GamepadButton::START,
	GamepadButton::BACK,

	GamepadButton::L1,
	GamepadButton::R1,

	GamepadButton::PAD_TOP,
	GamepadButton::PAD_DOWN,
	GamepadButton::PAD_LEFT,
	GamepadButton::PAD_RIGHT
};


void InputManager::AddListener(IInputListener* listener) {
	if (!listeners.ContainsElement(listener))
		listeners.Insert(listener);
}

void InputManager::RemoveListener(IInputListener* listener) {
	listeners.Remove(listener);
}

void InputManager::_Update(const Window& window) {
	// Key states
	for (TSize ikey = 0; ikey < (TSize)Key::__END; ikey++) {
		const Key key = (Key)ikey;

		// Pressed
		if (window.GetKeyboardState()->IsKeyDown(key)) {
			for (auto& listener : listeners)
				listener->OnKeyBeingPressed(key);

			// Stroke
			if (window.GetPreviousKeyboardState()->IsKeyUp(key))
				for (auto& listener : listeners)
					listener->OnKeyStroke(key);
		}
		// Released
		else {
			if (window.GetPreviousKeyboardState()->IsKeyDown(key))
				for (auto& listener : listeners)
					listener->OnKeyReleased(key);
		}
	}

	// Mouse states
	for (TSize ikey = 0; ikey < (TSize)MouseButton::__END; ikey++) {
		const MouseButton button = (MouseButton)ikey;

		// Pressed
		if (window.GetMouseState()->IsButtonDown(button)) {
			for (auto& listener : listeners)
				listener->OnMouseButtonBeingPressed(button);

			// Stroke
			if (window.GetPreviousMouseState()->IsButtonUp(button))
				for (auto& listener : listeners)
					listener->OnMouseButtonStroke(button);
		}
		// Released
		else {
			if (window.GetPreviousMouseState()->IsButtonDown(button))
				for (auto& listener : listeners)
					listener->OnMouseButtonReleased(button);
		}
	}

	// Mouse movement
	const Vector2i mouseDiff = window.GetMouseState()->GetPosition() - window.GetPreviousMouseState()->GetPosition();
	if (mouseDiff != Vector2i(0, 0))
		for (auto& listener : listeners)
			listener->OnMouseMoved(mouseDiff);

	// Mouse wheel movement
	const Vector2i wheelDiff = {
		window.GetMouseState()->GetHorizontalScroll() - window.GetPreviousMouseState()->GetHorizontalScroll(),
		window.GetMouseState()->GetScroll() - window.GetPreviousMouseState()->GetScroll()
	};
	if (wheelDiff != Vector2i(0, 0))
		for (auto& listener : listeners)
			listener->OnMouseWheelMoved(wheelDiff);

	// Gamepads
	for (TSize gamepad = 0; gamepad < 3; gamepad++) {
		if (!window.GetGamepadState(gamepad).IsConnected())
			continue;

		// Buttons
		for (TSize i = 0; i < _countof(gamepadButtons); i++) {
			const GamepadButton button = gamepadButtons[i];

			// Pressed
			if (window.GetGamepadState(gamepad).IsButtonDown(button)) {
				for (auto& listener : listeners)
					listener->OnGamepadButtonBeingPressed(button, gamepad);

				// Stroke
				if (window.GetGamepadState(gamepad).IsButtonUp(button))
					for (auto& listener : listeners)
						listener->OnGamepadButtonStroke(button, gamepad);
			}
			// Released
			else {
				if (window.GetPreviousGamepadState(gamepad).IsButtonDown(button))
					for (auto& listener : listeners)
						listener->OnGamepadButtonReleased(button, gamepad);
			}
		}

		// Axes
		if (!window.GetGamepadState(gamepad).IsConnected())
			return;

		for (TSize i = 0; i < (TSize)GamepadAxis::__END; i++) {
			const GamepadAxis axis = (GamepadAxis)i;

			const float originalValue = window.GetGamepadState(gamepad).GetAxisState(axis);

			for (auto& listener : listeners) {
				if (glm::abs(originalValue) > listener->GetMinAxisValue()) {
					listener->OnGamepadAxisValue(axis, originalValue, gamepad);
				}
				else {
					if (glm::abs(window.GetPreviousGamepadState(gamepad).GetAxisState(axis)) > listener->GetMinAxisValue())
						listener->OnGamepadAxisValue(axis, 0.0f, gamepad);
				}
			}

		}
		
	}

}
