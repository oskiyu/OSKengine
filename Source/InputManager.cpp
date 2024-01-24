#include "InputManager.h"

#include "IInputListener.h"

#include "IUserInput.h"

#include "IKeyboardInput.h"
#include "IMouseInput.h"
#include "IGamepadInput.h"

using namespace OSK;
using namespace OSK::IO;


// Gamepad buttons
const static GamepadButton gamepadButtons[] = {
	GamepadButton::x,
	GamepadButton::y,
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

void InputManager::_Update(const IUserInput& input) {
	const IKeyboardInput* keyboard = nullptr;
	const IMouseInput* mouse = nullptr;
	const IGamepadInput* gamepad = nullptr;

	input.QueryConstInterface(IUUID::IKeyboardInput, (const void**)&keyboard);
	input.QueryConstInterface(IUUID::IMouseInput, (const void**)&mouse);
	input.QueryConstInterface(IUUID::IGamepadInput, (const void**)&gamepad);

	if (keyboard) {
		// Key states
		for (UIndex32 ikey = 0; ikey < (UIndex32)Key::__END; ikey++) {
			const auto key = (Key)ikey;

			// Pressed
			if (keyboard->IsKeyDown(key)) {
				for (auto& listener : listeners)
					listener->OnKeyBeingPressed(key);

				// Stroke
				if (keyboard->GetPreviousKeyboardState().IsKeyUp(key))
					for (auto& listener : listeners)
						listener->OnKeyStroke(key);
			}
			// Released
			else {
				if (keyboard->GetPreviousKeyboardState().IsKeyDown(key))
					for (auto& listener : listeners)
						listener->OnKeyReleased(key);
			}
		}
	}


	if (mouse) {
		// Mouse states
		for (UIndex32 ikey = 0; ikey < (UIndex32)MouseButton::__END; ikey++) {
			const MouseButton button = (MouseButton)ikey;

			// Pressed
			if (mouse->GetMouseState().IsButtonDown(button)) {
				for (auto& listener : listeners)
					listener->OnMouseButtonBeingPressed(button);

				// Stroke
				if (mouse->GetPreviousMouseState().IsButtonUp(button))
					for (auto& listener : listeners)
						listener->OnMouseButtonStroke(button);
			}
			// Released
			else {
				if (mouse->GetPreviousMouseState().IsButtonDown(button))
					for (auto& listener : listeners)
						listener->OnMouseButtonReleased(button);
			}
		}

		// Mouse movement
		const Vector2i mouseDiff = mouse->GetMouseState().GetPosition() - mouse->GetPreviousMouseState().GetPosition();
		if (mouseDiff != Vector2i(0, 0))
			for (auto& listener : listeners)
				listener->OnMouseMoved(mouseDiff);

		// Mouse wheel movement
		const Vector2i wheelDiff = {
			mouse->GetMouseState().GetHorizontalScroll() - mouse->GetPreviousMouseState().GetHorizontalScroll(),
			mouse->GetMouseState().GetScroll() - mouse->GetPreviousMouseState().GetScroll()
		};
		if (wheelDiff != Vector2i(0, 0))
			for (auto& listener : listeners)
				listener->OnMouseWheelMoved(wheelDiff);
	}


	if (gamepad) {
	// Gamepads
		for (UIndex32 gamepadId = 0; gamepadId < 3; gamepadId++) {
			if (!gamepad->GetGamepadState(gamepadId).IsConnected())
				continue;

			// Buttons
			for (UIndex32 i = 0; i < _countof(gamepadButtons); i++) {
				const GamepadButton button = gamepadButtons[i];

				// Pressed
				if (gamepad->GetGamepadState(gamepadId).IsButtonDown(button)) {
					for (auto& listener : listeners)
						listener->OnGamepadButtonBeingPressed(button, gamepadId);

					// Stroke
					if (gamepad->GetGamepadState(gamepadId).IsButtonUp(button))
						for (auto& listener : listeners)
							listener->OnGamepadButtonStroke(button, gamepadId);
				}
				// Released
				else {
					if (gamepad->GetPreviousGamepadState(gamepadId).IsButtonDown(button))
						for (auto& listener : listeners)
							listener->OnGamepadButtonReleased(button, gamepadId);
				}
			}

			// Axes
			if (!gamepad->GetGamepadState(gamepadId).IsConnected())
				return;

			for (UIndex32 i = 0; i < (UIndex32)GamepadAxis::__END; i++) {
				const GamepadAxis axis = (GamepadAxis)i;

				const float originalValue = gamepad->GetGamepadState(gamepadId).GetAxisState(axis);

				for (auto& listener : listeners) {
					if (glm::abs(originalValue) > listener->GetMinAxisValue()) {
						listener->OnGamepadAxisValue(axis, originalValue, gamepadId);
					}
					else {
						if (glm::abs(gamepad->GetPreviousGamepadState(gamepadId).GetAxisState(axis)) > listener->GetMinAxisValue())
							listener->OnGamepadAxisValue(axis, 0.0f, gamepadId);
					}
				}

			}
		}
		
	}

}
