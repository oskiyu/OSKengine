#include "Window.h"

#include <GLFW/glfw3.h>

#include "MouseState.h"
#include "KeyboardState.h"
#include "RenderApiType.h"
#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"

using namespace OSK;

Window::Window() {
	glfwInit();

	oldMouseState = new MouseState;
	newMouseState = new MouseState;

	oldKeyboardState = new KeyboardState;
	newKeyboardState = new KeyboardState;
}

Window::~Window() {
	glfwTerminate();
}

void Window::SetRenderApiType(RenderApiType type) {
	renderApi = type;

	if (type == RenderApiType::OPENGL) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	else {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
}

void Window::Create(int32_t sizeX, int32_t sizeY, const std::string& title) {
	// Creación de la ventana.
	window = glfwCreateWindow(sizeX, sizeY, title.c_str(), NULL, NULL);
	OSK_ASSERT(window.HasValue(), "No se ha podido iniciar la ventana.");
	glfwMakeContextCurrent(window.GetPointer());

	this->sizeX = sizeX;
	this->sizeY = sizeY;

	UpdateScreenRatio();

	// Obtención de variables de glfw.
	glfwSetWindowUserPointer(window.GetPointer(), this);
	monitor = glfwGetPrimaryMonitor();
	monitorInfo = glfwGetVideoMode(monitor.GetPointer());

	// Establece los callbacks.
	glfwSetFramebufferSizeCallback(window.GetPointer(), Window::GlfwResizeCallback);
	glfwSetCursorPosCallback(window.GetPointer(), Window::GlfwMouseInputCallback);
	glfwSetScrollCallback(window.GetPointer(), Window::GlfwMouseScrollCallback);

	// Info-log.
	Engine::GetLogger()->InfoLog("Ventana iniciada correctamente.");
	Engine::GetLogger()->InfoLog("	Monitor sizeX: " + std::to_string(monitorInfo->width));
	Engine::GetLogger()->InfoLog("	Monitor sizeY: " + std::to_string(monitorInfo->height));
	Engine::GetLogger()->InfoLog("	Monitor refresh rate: " + std::to_string(monitorInfo->refreshRate));
}

void Window::Update() {
	glfwPollEvents();

	if (renderApi == RenderApiType::OPENGL)
		glfwSwapBuffers(window.GetPointer());

	*oldKeyboardState = *newKeyboardState;
	UpdateKeyboardState(newKeyboardState.GetPointer());

	*oldMouseState = *newMouseState;
	UpdateMouseState(newMouseState.GetPointer());
}

void Window::SetFullScreen(bool fullscreen) {
	if (isFullScreen == fullscreen)
		return;

	if (fullscreen) {
		// Obtenemos la posición de la ventana,
		// para poder reestablecerla después de salir de
		// la ventana completa.
		glfwGetWindowPos(window.GetPointer(), &previous.posX, &previous.posY);
		glfwGetWindowSize(window.GetPointer(), &previous.sizeX, &previous.sizeY);

		glfwSetWindowMonitor(window.GetPointer(), monitor.GetPointer(), 0, 0, monitorInfo->width, monitorInfo->height, monitorInfo->refreshRate);

		sizeX = monitorInfo->width;
		sizeY = monitorInfo->height;

		UpdateScreenRatio();
	}
	else {
		glfwSetWindowMonitor(window.GetPointer(), nullptr, previous.posX, previous.posY, previous.sizeX, previous.sizeY, monitorInfo->refreshRate);
	}

	isFullScreen = fullscreen;
}

void Window::ToggleFullScreen() {
	SetFullScreen(!isFullScreen);
}

bool Window::IsFullScreen() const {
	return isFullScreen;
}

void Window::SetMousePosition(int32_t posX, int32_t posY) {
	glfwSetCursorPos(window.GetPointer(), posX, posY);
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(window.GetPointer());
}

void Window::Close() {
	glfwSetWindowShouldClose(window.GetPointer(), true);
}

Vector2ui Window::GetWindowSize() const {
	return { sizeX, sizeY };
}
float Window::GetScreenRatio() const {
	return screenRatio;
}

int32_t Window::GetRefreshRate() const {
	return monitorInfo->refreshRate;
}

const KeyboardState* Window::GetKeyboardState() const {
	return newKeyboardState.GetPointer();
}

const KeyboardState* Window::GetPreviousKeyboardState() const {
	return oldKeyboardState.GetPointer();
}

const MouseState* Window::GetMouseState() const {
	return newMouseState.GetPointer();
}

const MouseState* Window::GetPreviousMouseState() const {
	return oldMouseState.GetPointer();
}

GLFWwindow* Window::_GetGlfw() const {
	return window.GetPointer();
}

Window* Window::GetWindowForCallback(GLFWwindow* window) {
	void* ptr = glfwGetWindowUserPointer(window);
	if (auto winPtr = (Window*)ptr)
		return winPtr;

	return nullptr;
}

void Window::GlfwMouseInputCallback(GLFWwindow* window, double posX, double posY) {
	GetWindowForCallback(window)->MouseInputCallback(posX, posY);
}

void Window::MouseInputCallback(double posX, double posY) {
	newMouseState->_SetPosition(Vector2d(posX, posY).ToVector2i());
	newMouseState->_SetRelativePosition(Vector2d(posX, posY).ToVector2f() / GetWindowSize().ToVector2f());
}

void Window::GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey) {
	GetWindowForCallback(window)->ResizeCallback(sizex, sizey);
}

void Window::ResizeCallback(int sizex, int sizey) {
	sizeX = sizex;
	sizeY = sizey;

	if (renderApi == RenderApiType::OPENGL)
		glViewport(0, 0, GetWindowSize().X, GetWindowSize().Y);
}
void Window::GlfwMouseScrollCallback(GLFWwindow* window, double dX, double dY) {
	GetWindowForCallback(window)->MouseScrollCallback(dX, dY);
}

void Window::MouseScrollCallback(double dX, double dY) {
	newMouseState->_SetScrollX((int)dX);
	newMouseState->_SetScrollY((int)dY);
}

void Window::UpdateScreenRatio() {
	if (sizeY != 0)
		screenRatio = (float)sizeX / sizeY;
}

void Window::UpdateKeyboardState(KeyboardState* keyboard) {
	keyboard->_SetKeyState(Key::SPACE, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_SPACE));
	keyboard->_SetKeyState(Key::APOSTROPHE, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_APOSTROPHE));

	for (int i = 0; i < 14; i++)
		keyboard->_SetKeyState((Key)((int)Key::COMMA + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_COMMA + i));

	keyboard->_SetKeyState(Key::SEMICOLON, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_SEMICOLON));
	keyboard->_SetKeyState(Key::EQUAL, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_EQUAL));

	for (int i = 0; i < 29; i++)
		keyboard->_SetKeyState((Key)((int)Key::A + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_A + i));

	keyboard->_SetKeyState(Key::GRAVE_ACCENT, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_GRAVE_ACCENT));

	for (int i = 0; i < 14; i++)
		keyboard->_SetKeyState((Key)((int)Key::ESCAPE + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_ESCAPE + i));

	for (int i = 0; i < 5; i++)
		keyboard->_SetKeyState((Key)((int)Key::CAPS_LOCK + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_CAPS_LOCK + i));

	for (int i = 0; i < 25; i++)
		keyboard->_SetKeyState((Key)((int)Key::F1 + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_F1 + i));

	for (int i = 0; i < 17; i++)
		keyboard->_SetKeyState((Key)((int)Key::KEYPAD_0 + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_KP_0 + i));

	for (int i = 0; i < 9; i++)
		keyboard->_SetKeyState((Key)((int)Key::LEFT_SHIFT + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_LEFT_SHIFT + i));
}

void Window::UpdateMouseState(MouseState* mouse) {
	for (int i = 0; i < 8; i++)
		mouse->_SetButtonState((MouseButton)i, (ButtonState)glfwGetMouseButton(window.GetPointer(), i));

	mouse->_SetButtonState(MouseButton::BUTTON_LEFT, mouse->GetButtonState(MouseButton::BUTTON_1));
	mouse->_SetButtonState(MouseButton::BUTTON_RIGHT, mouse->GetButtonState(MouseButton::BUTTON_2));
	mouse->_SetButtonState(MouseButton::BUTTON_MIDDLE, mouse->GetButtonState(MouseButton::BUTTON_3));
}
