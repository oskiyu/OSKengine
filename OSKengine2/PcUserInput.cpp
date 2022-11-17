// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "PcUserInput.h"

#include "OSKengine.h"
#include "Window.h"

using namespace OSK;
using namespace OSK::IO;

void PcUserInput::Initialize(const Window& window) {
	self = this;

	glfwSetCursorPosCallback(window._GetGlfw(), PcUserInput::MouseInputCallback);
	glfwSetScrollCallback(window._GetGlfw(), PcUserInput::MouseScrollCallback);
}

void PcUserInput::Update() {
	UpdateKeyboardInput();
	UpdateMouseInput();
	UpdateGamepadInput();
}

void PcUserInput::QueryInterface(TInterfaceUuid uuid, void** ptr) const {
	if (uuid == OSK_IUUID(IKeyboardInput)) {
		*ptr = (IKeyboardInput*)this;
	}
	else if (uuid == OSK_IUUID(IMouseInput)) {
		*ptr = (IMouseInput*)this;
	}
	else if (uuid == OSK_IUUID(IGamepadInput)) {
		*ptr = (IGamepadInput*)this;
	}
	else {
		*ptr = nullptr;
	}
}

void PcUserInput::UpdateKeyboardState(KeyboardState* keyboard) {
	GLFWwindow* window = Engine::GetDisplay()->As<Window>()->_GetGlfw();

	keyboard->_SetKeyState(Key::SPACE, (KeyState)glfwGetKey(window, GLFW_KEY_SPACE));
	keyboard->_SetKeyState(Key::APOSTROPHE, (KeyState)glfwGetKey(window, GLFW_KEY_APOSTROPHE));

	for (int i = 0; i < 14; i++)
		keyboard->_SetKeyState((Key)((int)Key::COMMA + i), (KeyState)glfwGetKey(window, GLFW_KEY_COMMA + i));

	keyboard->_SetKeyState(Key::SEMICOLON, (KeyState)glfwGetKey(window, GLFW_KEY_SEMICOLON));
	keyboard->_SetKeyState(Key::EQUAL, (KeyState)glfwGetKey(window, GLFW_KEY_EQUAL));

	for (int i = 0; i < 29; i++)
		keyboard->_SetKeyState((Key)((int)Key::A + i), (KeyState)glfwGetKey(window, GLFW_KEY_A + i));

	keyboard->_SetKeyState(Key::GRAVE_ACCENT, (KeyState)glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT));

	for (int i = 0; i < 14; i++)
		keyboard->_SetKeyState((Key)((int)Key::ESCAPE + i), (KeyState)glfwGetKey(window, GLFW_KEY_ESCAPE + i));

	for (int i = 0; i < 5; i++)
		keyboard->_SetKeyState((Key)((int)Key::CAPS_LOCK + i), (KeyState)glfwGetKey(window, GLFW_KEY_CAPS_LOCK + i));

	for (int i = 0; i < 25; i++)
		keyboard->_SetKeyState((Key)((int)Key::F1 + i), (KeyState)glfwGetKey(window, GLFW_KEY_F1 + i));

	for (int i = 0; i < 17; i++)
		keyboard->_SetKeyState((Key)((int)Key::KEYPAD_0 + i), (KeyState)glfwGetKey(window, GLFW_KEY_KP_0 + i));

	for (int i = 0; i < 9; i++)
		keyboard->_SetKeyState((Key)((int)Key::LEFT_SHIFT + i), (KeyState)glfwGetKey(window, GLFW_KEY_LEFT_SHIFT + i));
}

void PcUserInput::UpdateMouseState(MouseState* state) {
	*state = mouseCallbackState;
	
	GLFWwindow* window = Engine::GetDisplay()->As<Window>()->_GetGlfw();

	for (int i = 0; i < 8; i++)
		state->_SetButtonState((MouseButton)i, (ButtonState)glfwGetMouseButton(window, i));

	state->_SetButtonState(MouseButton::BUTTON_LEFT, state->GetButtonState(MouseButton::BUTTON_1));
	state->_SetButtonState(MouseButton::BUTTON_RIGHT, state->GetButtonState(MouseButton::BUTTON_2));
	state->_SetButtonState(MouseButton::BUTTON_MIDDLE, state->GetButtonState(MouseButton::BUTTON_3));
}

void PcUserInput::UpdateGamepadState(GamepadState* state) {
	const auto id = GLFW_JOYSTICK_1 + state->GetIdentifier();

	if (glfwJoystickPresent(id)) {
		int count;
		const float* axes = glfwGetJoystickAxes(id, &count);

		state->_SetConnectionState(true);

		for (int axis = 0; axis < 6; axis++)
			state->_SetAxisState((GamepadAxis)axis, axes[axis]);

		const unsigned char* buttons = glfwGetJoystickButtons(id, &count);
		for (int button = 0; button < 4; button++)
			state->_SetButtonState((GamepadButton)button, (bool)buttons[button] ? GamepadButtonState::PRESSED : GamepadButtonState::RELEASED);
	}
	else {
		state->_SetConnectionState(false);
	}
}

void PcUserInput::SetupMotionMode() {
	GLFWwindow* window = Engine::GetDisplay()->As<Window>()->_GetGlfw();

	if (GetMotionMode() == MouseMotionMode::ACCELERATED) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
	else {
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		else
			OSK_CHECK(false, "Raw mouse mode no soportado.");
	}
}

void PcUserInput::SetupReturnMode() {
	GLFWwindow* window = Engine::GetDisplay()->As<Window>()->_GetGlfw();

	if (GetReturnMode() == MouseReturnMode::ALWAYS_RETURN)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else if (GetReturnMode() == MouseReturnMode::FREE)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void PcUserInput::SetMousePosition(const Vector2ui& pos) {
	glfwSetCursorPos(Engine::GetDisplay()->As<Window>()->_GetGlfw(), pos.X, pos.Y);
}

void PcUserInput::MouseInputCallback(GLFWwindow* window, double dX, double dY) {
	self->mouseCallbackState._SetPosition(Vector2d(dX, dY).ToVector2i());
	self->mouseCallbackState._SetRelativePosition(Vector2d(dX, dY).ToVector2f()
		/ Engine::GetDisplay()->GetResolution().ToVector2f());
}

void PcUserInput::MouseScrollCallback(GLFWwindow* window, double dX, double dY) {
	self->mouseCallbackState._SetScrollX(dX);
	self->mouseCallbackState._SetScrollY(dX);
}

PcUserInput* PcUserInput::self;
