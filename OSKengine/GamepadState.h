#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include <GLFW/glfw3.h>

#include <array>

namespace OSK {

	enum class GamepadAxis {

		LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X,
		LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
		RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
		RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
		L2 = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		R2 = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER

	};

	enum class GamepadButton {

		X = GLFW_GAMEPAD_BUTTON_X,
		Y = GLFW_GAMEPAD_BUTTON_Y,
		A = GLFW_GAMEPAD_BUTTON_A,
		B = GLFW_GAMEPAD_BUTTON_B,

		START = GLFW_GAMEPAD_BUTTON_START,
		BACK = GLFW_GAMEPAD_BUTTON_BACK,

		L1 = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		R1 = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,

		PAD_TOP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
		PAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		PAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
		PAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT

	};

	struct OSKAPI_CALL GamepadState {

		friend class WindowAPI;

	public:

		buttonState_t GetButtonState(GamepadButton button) const;

		bool IsButtonDown(GamepadButton button) const;
		bool IsButtonUp(GamepadButton button) const;

		float GetAxisState(GamepadAxis axis) const;

		gamepadId_t GetID() const;

		bool IsConnected() const;

	private:

		std::array<int, 4> buttonStates;
		std::array<float, 6> axesStates;

		gamepadId_t identifier = 0;

		bool isConnected = false;

	};

}
