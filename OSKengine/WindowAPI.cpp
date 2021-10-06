#include "WindowAPI.h"

#include "stbi_image.h"

#include "Camera2D.h"

namespace OSK {

	WindowAPI::WindowAPI() {
		glfwInit();

		screenSizeX = 0;
		screenSizeY = 0;
		screenRatio = 0;

		window = nullptr;
	}


	WindowAPI::~WindowAPI() {
		glfwTerminate();
	}


	result_t WindowAPI::SetWindow(int32_t sizeX, int32_t sizeY, const std::string& name) {
		//Característias de la ventana
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Evita que use OpenGL
		
		//Crear ventana
		window = glfwCreateWindow(sizeX, sizeY, name.c_str(), NULL, NULL);
		if (window.GetPointer() == NULL) {
			glfwTerminate();
			return false;
		}

		screenSizeX = sizeX;
		screenSizeY = sizeY;
		
		posXbackup = 0;
		posYbackup = 0;
		sizeXbackup = sizeX;
		sizeYbackup = sizeY;

		if (screenSizeY != 0)
			screenRatio = static_cast<float_t>(sizeX) / static_cast<float_t>(sizeY);

		glfwSetWindowUserPointer(window.GetPointer(), this);

		monitor = glfwGetPrimaryMonitor();

		glfwSetFramebufferSizeCallback(window.GetPointer(), WindowAPI::defaultFramebufferSizeCallback);
		glfwSetCursorPosCallback(window.GetPointer(), mouseInputCallback);
		glfwSetScrollCallback(window.GetPointer(), mouseScrollCallback);
			

#ifndef OSK_DEBUG
		//SetFullscreen(true);
#endif // !OSK_DEBUG

		return true;
	}

	bool WindowAPI::WindowShouldClose() {
		updateMouseButtons();

		return glfwWindowShouldClose(window.GetPointer());
	}

	void WindowAPI::PollEvents() {
		glfwPollEvents();

		mouseState.oldScrollX = mouseState.scrollX;
		mouseState.oldScrollY = mouseState.scrollY;

		for (int i = 0; i < 4; i++) {
			const auto id = GLFW_JOYSTICK_1 + i;

			if (glfwJoystickPresent(id)) {
				int count;
				const float* axes = glfwGetJoystickAxes(id, &count);

				gamepadStates[i].identifier = id;
				gamepadStates[i].isConnected = true;

				for (int axis = 0; axis < 6; axis++) {
					float diff = axes[axis];
					if (glm::abs(diff) < 0.25f)
						diff = 0.0f;

					gamepadStates[i].axesStates[axis] = diff;
				}

				const unsigned char* buttons = glfwGetJoystickButtons(id, &count);
				for (int button = 0; button < 4; button++)
					gamepadStates[i].buttonStates[button] = buttons[button];
			}
			else {
				gamepadStates[i].isConnected = false;
			}
		}
	}


	void WindowAPI::UpdateKeyboardState(KeyboardState& keyboard) {
		keyboard.keyStates[static_cast<keyCode_t>(Key::SPACE)] = glfwGetKey(window.GetPointer(), GLFW_KEY_SPACE);
		keyboard.keyStates[static_cast<keyCode_t>(Key::APOSTROPHE)] = glfwGetKey(window.GetPointer(), GLFW_KEY_APOSTROPHE);
		
		for (keyCode_t i = 0; i < 14; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::COMMA) + i] = glfwGetKey(window.GetPointer(), GLFW_KEY_COMMA + i);
		}

		keyboard.keyStates[static_cast<keyCode_t>(Key::SEMICOLON)] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_SEMICOLON));
		keyboard.keyStates[static_cast<keyCode_t>(Key::EQUAL)] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_EQUAL));

		for (keyCode_t i = 0; i < 29; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::A) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_A + i));
		}

		keyboard.keyStates[static_cast<keyCode_t>(Key::GRAVE_ACCENT)] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_GRAVE_ACCENT));
	
		for (keyCode_t i = 0; i < 14; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::ESCAPE) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_ESCAPE + i));
		}

		for (keyCode_t i = 0; i < 5; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::CAPS_LOCK) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_CAPS_LOCK + i));
		}
		
		for (keyCode_t i = 0; i < 25; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::F1) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_F1 + i));
		}

		for (keyCode_t i = 0; i < 17; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::KEYPAD_0) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_KP_0 + i));
		}

		for (keyCode_t i = 0; i < 9; i++) {
			keyboard.keyStates[static_cast<keyCode_t>(Key::LEFT_SHIFT) + i] = static_cast<keyState_t>(glfwGetKey(window.GetPointer(), GLFW_KEY_LEFT_SHIFT + i));
		}
	}


	void WindowAPI::UpdateMouseState(MouseState& mouseState) {
		mouseState = this->mouseState;
	}


	void WindowAPI::SetMousePosition(int32_t posX, int32_t posY) {
		glfwSetCursorPos(window.GetPointer(), posX, posY);
	}


	void WindowAPI::SetMouseMode(MouseInputMode mode) {
		if (mode == MouseInputMode::ALWAYS_RETURN)
			glfwSetInputMode(window.GetPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else if (mode == MouseInputMode::FREE)
			glfwSetInputMode(window.GetPointer(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	void WindowAPI::SetMouseVisibility(MouseVisibility mode) {
		if (mode == MouseVisibility::VISIBLE)
			glfwSetInputMode(window.GetPointer(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else if (mode == MouseVisibility::HIDDEN)
			glfwSetInputMode(window.GetPointer(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}


	void WindowAPI::SetMouseMovementMode(MouseAccelerationMode mode) {
		if (mode == MouseAccelerationMode::RAW) {
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(window.GetPointer(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			else
				Logger::Log(LogMessageLevels::WARNING, "raw mouse mode no soportado.", __LINE__);
		} 
		else if (mode == MouseAccelerationMode::ENABLE_PRECISSION) {
			glfwSetInputMode(window.GetPointer(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}


	void WindowAPI::SetFullscreen(bool fullscreen) {
		if (isFullscreen == fullscreen)
			return;

		if (fullscreen) {
			glfwGetWindowPos(window.GetPointer(), &posXbackup, &posYbackup);
			glfwGetWindowSize(window.GetPointer(), &sizeXbackup, &sizeYbackup);

			const GLFWvidmode* mode = glfwGetVideoMode(monitor.GetPointer());
			glfwSetWindowMonitor(window.GetPointer(), monitor.GetPointer(), 0, 0, mode->width, mode->height, 60);

			resize(mode->width, mode->height);
		}
		else {
			glfwSetWindowMonitor(window.GetPointer(), nullptr, posXbackup, posYbackup, sizeXbackup, sizeYbackup, 60);
			
			resize(sizeXbackup, sizeYbackup);
		}

		isFullscreen = fullscreen;
	}


	void WindowAPI::Close() {
		glfwSetWindowShouldClose(window.GetPointer(), true);
	}

	Vector4 WindowAPI::GetRectangle() const {
		return Vector4(0, 0, (float)screenSizeX, (float)screenSizeY);
	}


	void WindowAPI::CenterMouse() {
		SetMousePosition(screenSizeX / 2, screenSizeY / 2);
	}


	deltaTime_t WindowAPI::GetTime() {
		return static_cast<deltaTime_t>(glfwGetTime());
	}


	void WindowAPI::defaultFramebufferSizeCallback(GLFWwindow* win, int sizex, int sizey) {
		void* ptr = glfwGetWindowUserPointer(win);
		if (WindowAPI * wndPtr = static_cast<OSK::WindowAPI*>(ptr))
			wndPtr->resize(sizex, sizey);
	}


	void WindowAPI::resize(double_t sizex, double_t sizey) {
		screenSizeX = (int32_t)sizex;
		screenSizeY = (int32_t)sizey;

		if (sizey != 0)
			screenRatio = static_cast<float>(sizex) / static_cast<float>(sizey);
	}


	void WindowAPI::mouseInputCallback(GLFWwindow* window, double posX, double posY) {
		void* ptr = glfwGetWindowUserPointer(window);
		if (WindowAPI * wndPtr = static_cast<WindowAPI*>(ptr))
			wndPtr->mouseInput(posX, posY);
	}


	void WindowAPI::mouseInput(double_t posX, double_t posY) {
		mouseState.positionX = (mouseVar_t)posX;
		mouseState.positionY = (mouseVar_t)posY;
		mouseState.relativePositionX = mouseState.positionX / (mouseVar_t)screenSizeX;
		mouseState.relativePositionY = mouseState.positionY / (mouseVar_t)screenSizeY;
	}


	void WindowAPI::mouseScrollCallback(GLFWwindow* window, double dX, double dY) {
		void* ptr = glfwGetWindowUserPointer(window);
		if (WindowAPI * wndPtr = static_cast<WindowAPI*>(ptr))
			wndPtr->mouseScroll(dX, dY);
	}


	void WindowAPI::mouseScroll(double_t dX, double_t dY) {
		mouseState.oldScrollX = mouseState.scrollX;
		mouseState.oldScrollY = mouseState.scrollY;

		mouseState.scrollX += static_cast<float_t>(dX);
		mouseState.scrollY += static_cast<float_t>(dY);
	}


	void WindowAPI::updateMouseButtons() {
		for (buttonCode_t i = 0; i < 8; i++)
			mouseState.buttonStates[i] = static_cast<buttonState_t>(glfwGetMouseButton(window.GetPointer(), i));
	}

	Vector2i WindowAPI::GetSize() const {
		return { screenSizeX, screenSizeY };
	}

	/// <summary>
	/// Ratio de aspecto de la ventana.
	/// En porcentaje de X respecto a Y (x/y).
	/// </summary>
	float WindowAPI::GetScreenAspectRatio() const {
		return screenRatio;
	}

	/// <summary>
	/// True si está en pantalla completa.
	/// </summary>
	bool WindowAPI::IsFullscreen() const {
		return isFullscreen;
	}

}