#include "WindowAPI.h"

#include "BaseUIElement.h"
#include "IUserInterfaceHasChildren.h"
#include "IUserInterfaceMouseInput.h"
#include "IUserInterfaceImage.h"

#include "stbi_image.h"

namespace OSK {

	WindowAPI::WindowAPI() {
		glfwInit();

		ScreenSizeX = 0;
		ScreenSizeY = 0;
		ScreenRatio = 0;

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
		if (window == NULL) {
			glfwTerminate();
			return false;
		}

		ScreenSizeX = sizeX;
		ScreenSizeY = sizeY;
		
		posXbackup = 0;
		posYbackup = 0;
		sizeXbackup = sizeX;
		sizeYbackup = sizeY;

		if (ScreenSizeY != 0)
			ScreenRatio = static_cast<float_t>(sizeX) / static_cast<float_t>(sizeY);

		glfwSetWindowUserPointer(window, this);

		monitor = glfwGetPrimaryMonitor();

		glfwSetFramebufferSizeCallback(window, WindowAPI::defaultFramebufferSizeCallback);
		glfwSetCursorPosCallback(window, mouseInputCallback);
		glfwSetScrollCallback(window, mouseScrollCallback);

		

#ifndef OSK_DEBUG
		//SetFullscreen(true);
#endif // !OSK_DEBUG

		return true;
	}

	/*void WindowAPI::SetWindowIcon(const std::string& iconPath) {
		GLFWimage icons[1];
		int sizeX;
		int sizeY;
		int nChannels;
		icons[0].pixels = stbi_load(iconPath.c_str(), &sizeX, &sizeY, 0, 4);
		glfwSetWindowIcon(window, 1, icons);
		stbi_image_free(icons[0].pixels);
	}*/

	bool WindowAPI::WindowShouldClose() {
		updateMouseButtons();

		return glfwWindowShouldClose(window);
	}


	void WindowAPI::SwapBuffers() {
		glfwSwapBuffers(window);
	}


	void WindowAPI::PollEvents() {
		glfwPollEvents();

		updateUI(UserInterface);
		
		mouseState.OldScrollX = mouseState.ScrollX;
		mouseState.OldScrollY = mouseState.ScrollY;
	}


	void WindowAPI::UpdateKeyboardState(KeyboardState& keyboard) {
		keyboard.KeyStates[static_cast<keyCode_t>(Key::SPACE)] = glfwGetKey(window, GLFW_KEY_SPACE);
		keyboard.KeyStates[static_cast<keyCode_t>(Key::APOSTROPHE)] = glfwGetKey(window, GLFW_KEY_APOSTROPHE);
		
		for (keyCode_t i = 0; i < 14; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::COMMA) + i] = glfwGetKey(window, GLFW_KEY_COMMA + i);
		}

		keyboard.KeyStates[static_cast<keyCode_t>(Key::SEMICOLON)] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_SEMICOLON));
		keyboard.KeyStates[static_cast<keyCode_t>(Key::EQUAL)] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_EQUAL));

		for (keyCode_t i = 0; i < 29; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::A) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_A + i));
		}

		keyboard.KeyStates[static_cast<keyCode_t>(Key::GRAVE_ACCENT)] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT));
	
		for (keyCode_t i = 0; i < 14; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::ESCAPE) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_ESCAPE + i));
		}

		for (keyCode_t i = 0; i < 5; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::CAPS_LOCK) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_CAPS_LOCK + i));
		}
		
		for (keyCode_t i = 0; i < 25; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::F1) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_F1 + i));
		}

		for (keyCode_t i = 0; i < 17; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::KEYPAD_0) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_KP_0 + i));
		}

		for (keyCode_t i = 0; i < 9; i++) {
			keyboard.KeyStates[static_cast<keyCode_t>(Key::LEFT_SHIFT) + i] = static_cast<keyState_t>(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT + i));
		}
	}


	void WindowAPI::UpdateMouseState(MouseState& mouseState) {
		mouseState = this->mouseState;
	}


	void WindowAPI::SetMousePosition(int32_t posX, int32_t posY) {
		glfwSetCursorPos(window, posX, posY);
	}


	void WindowAPI::SetMouseMode(MouseInputMode mode) {
		if (mode == MouseInputMode::ALWAYS_RETURN)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else if (mode == MouseInputMode::NORMAL)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	void WindowAPI::SetMouseVisibility(MouseVisibility mode) {
		if (mode == MouseVisibility::VISIBLE)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else if (mode == MouseVisibility::HIDDEN)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}


	void WindowAPI::SetMouseMovementMode(MouseMovementMode mode) {
		if (mode == MouseMovementMode::RAW) {
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			else
				Logger::Log(LogMessageLevels::WARNING, "raw mouse mode no soportado.", __LINE__);
		} 
		else if (mode == MouseMovementMode::ENABLE_PRECISSION) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}


	void WindowAPI::SetFullscreen(bool fullscreen) {
		if (IsFullscreen == fullscreen)
			return;

		if (fullscreen) {
			glfwGetWindowPos(window, &posXbackup, &posYbackup);
			glfwGetWindowSize(window, &sizeXbackup, &sizeYbackup);

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 60);

			resize(mode->width, mode->height);
		}
		else {
			glfwSetWindowMonitor(window, nullptr, posXbackup, posYbackup, sizeXbackup, sizeYbackup, 60);
			
			resize(sizeXbackup, sizeYbackup);
		}

		IsFullscreen = fullscreen;
	}


	void WindowAPI::Close() {
		glfwSetWindowShouldClose(window, true);
	}


	void WindowAPI::SetMousePosition(uint32_t x, uint32_t y) {
		glfwSetCursorPos(window, x, y);
	}


	void WindowAPI::SetUserInterface(UI::BaseUIElement* ui) {
		UserInterface = ui;
		
		if (ui->FillParent) {
			ui->SetRectangle(Vector4(0, 0, ScreenSizeX, ScreenSizeY));
			/*if (auto var = reinterpret_cast<UI::IUserInterfaceImage*>(ui)) {
				var->SetSprite(var->Image);
			}*/
		}
	}


	Vector4 WindowAPI::GetRectangle() const {
		return Vector4(0, 0, ScreenSizeX, ScreenSizeY);
	}


	void WindowAPI::CenterMouse() {
		SetMousePosition(ScreenSizeX / 2, ScreenSizeY / 2);
	}


	deltaTime_t WindowAPI::GetTime() {
		return static_cast<deltaTime_t>(glfwGetTime());
	}


	GLFWwindow* WindowAPI::GetGLFWWindow() const {
		return window;
	}


	void WindowAPI::defaultFramebufferSizeCallback(GLFWwindow* win, int sizex, int sizey) {
		void* ptr = glfwGetWindowUserPointer(win);
		if (WindowAPI * wndPtr = static_cast<OSK::WindowAPI*>(ptr))
			wndPtr->resize(sizex, sizey);
	}


	void WindowAPI::resize(double_t sizex, double_t sizey) {
		ScreenSizeX = sizex;
		ScreenSizeY = sizey;

		if (sizey != 0)
			ScreenRatio = static_cast<float_t>(sizex) / static_cast<float_t>(sizey);

		if (UserInterface != nullptr) {
			if (UserInterface->FillParent)
				UserInterface->SetRectangle(Vector4(0, 0, ScreenSizeX, ScreenSizeY));

			UserInterface->SetPositionRelativeTo(nullptr);

			if (auto var = reinterpret_cast<UI::IUserInterfaceHasChildren*>(UserInterface))
				var->UpdateChildrenPositions();
		}

		ViewportShouldBeUpdated = true;
	}


	void WindowAPI::mouseInputCallback(GLFWwindow* window, double posX, double posY) {
		void* ptr = glfwGetWindowUserPointer(window);
		if (WindowAPI * wndPtr = static_cast<WindowAPI*>(ptr))
			wndPtr->mouseInput(posX, posY);
	}


	void WindowAPI::mouseInput(double_t posX, double_t posY) {
		mouseState.PositionX = posX;
		mouseState.PositionY = /*ScreenSizeY -*/ posY;
		mouseState.RelativePositionX = mouseState.PositionX / (double)ScreenSizeX;
		mouseState.RelativePositionY = mouseState.PositionY / (double)ScreenSizeY;
	}


	void WindowAPI::mouseScrollCallback(GLFWwindow* window, double dX, double dY) {
		void* ptr = glfwGetWindowUserPointer(window);
		if (WindowAPI * wndPtr = static_cast<WindowAPI*>(ptr))
			wndPtr->mouseScroll(dX, dY);
	}


	void WindowAPI::mouseScroll(double_t dX, double_t dY) {
		mouseState.ScrollX += static_cast<float_t>(dX);
		mouseState.ScrollY += static_cast<float_t>(dY);
	}


	void WindowAPI::updateMouseButtons() {
		for (buttonCode_t i = 0; i < 8; i++) {
			mouseState.ButtonStates[i] = static_cast<buttonState_t>(glfwGetMouseButton(window, i));
		}
	}


	void WindowAPI::updateUI(UI::BaseUIElement* element) {
		if (element == nullptr || !element->IsActive)
			return;

		if (auto var = dynamic_cast<UI::IUserInterfaceMouseInput*>(element)) {
			var->Update(mouseState);
		}

		if (auto var = dynamic_cast<UI::IUserInterfaceHasChildren*>(element)) {
			for (auto i : var->Children)
				updateUI(i);
		}
	}

}