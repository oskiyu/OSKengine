#include "Window.h"

#include <GLFW/glfw3.h>

#include "MouseState.h"
#include "KeyboardState.h"
#include "RenderApiType.h"
#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"
#include "IRenderer.h"
#include "MouseModes.h"
#include "PcUserInput.h"

#include <stbi_image.h>

#include <Windows.h>
#include "StringOperations.h"

#include "InitializeWindowException.h"

using namespace OSK;
using namespace OSK::IO;

Window::Window() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
}

Window::~Window() {
	glfwTerminate();
}


void Window::Create(Vector2ui size, const std::string& title) {

	// Creación de la ventana.
	window = UniquePtr<GLFWwindow, GlfwWindowDeleter>(glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL));
	OSK_ASSERT(window.HasValue(), InitializeWindowException());

	resolution = size;

	// Obtención de variables de glfw.
	glfwSetWindowUserPointer(window.GetPointer(), this);
	monitor = UniquePtr<GLFWmonitor, GlfwMonitorDeleter>(glfwGetPrimaryMonitor());
	monitorInfo = glfwGetVideoMode(monitor.GetPointer());

	refreshRate = monitorInfo->refreshRate;

	// Establece los callbacks.
	glfwSetFramebufferSizeCallback(window.GetPointer(), Window::GlfwResizeCallback);

	{
		int x = 0;
		int y = 0;
		int nmChannels = 0;
		stbi_uc* icon = stbi_load("Resources/Icons/engineIcon.png", &x, &y, &nmChannels, 4);

		GLFWimage glfwIcon{};
		glfwIcon.width = x;
		glfwIcon.height = y;
		glfwIcon.pixels = icon;

		glfwSetWindowIcon(window.GetPointer(), 1, std::addressof(glfwIcon));

		stbi_image_free(icon);
	}

	isOpen = true;

	// Si usamos Window, entonces estamos en PC, por lo que Input es PcUserInput.
	static_cast<PcUserInput*>(Engine::GetInput())->Initialize(*this);
}

void Window::QueryInterface(TInterfaceUuid uuid, void** ptr) const {
	if (uuid == OSK_IUUID(IFullscreenableDisplay))
		*ptr = (IFullscreenableDisplay*)this;
	else
		*ptr = nullptr;
}

void Window::QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const {
	if (uuid == OSK_IUUID(IFullscreenableDisplay))
		*ptr = (const IFullscreenableDisplay*)this;
	else
		*ptr = nullptr;
}

void Window::Update() {
	glfwPollEvents();
	isOpen = !glfwWindowShouldClose(window.GetPointer());
}

void Window::SetFullscreen(bool fullscreen) {
	if (isFullscreen == fullscreen)
		return;

	if (fullscreen) {
		// Obtenemos la posición de la ventana,
		// para poder reestablecerla después de salir de
		// la ventana completa.
		glfwGetWindowPos(window.GetPointer(), &previous.position.x, &previous.position.y);
		glfwGetWindowSize(window.GetPointer(), &previous.size.x, &previous.size.y);

		glfwSetWindowMonitor(window.GetPointer(), monitor.GetPointer(), 0, 0, monitorInfo->width, monitorInfo->height, monitorInfo->refreshRate);

		resolution.x = monitorInfo->width;
		resolution.y = monitorInfo->height;
	}
	else {
		glfwSetWindowMonitor(window.GetPointer(), nullptr, previous.position.x, previous.position.y, previous.size.x, previous.size.y, monitorInfo->refreshRate);
	}

	isFullscreen = fullscreen;
}

void Window::Close() {
	glfwSetWindowShouldClose(window.GetPointer(), true);
	isOpen = false;
}

GLFWwindow* Window::_GetGlfw() {
	return window.GetPointer();
}

void Window::ShowMessageBox(std::string_view msg) {
	const std::wstring widestring = StringToWideString((std::string)msg);	\
	MessageBox(NULL, widestring.c_str(), NULL, MB_OK | MB_ICONERROR);	\
}

Window* Window::GetWindowForCallback(GLFWwindow* window) {
	return static_cast<Window*>(glfwGetWindowUserPointer(window));
}

void Window::GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey) {
	GetWindowForCallback(window)->ResizeCallback(sizex, sizey);
}

void Window::ResizeCallback(int sizex, int sizey) {
	resolution.x = sizex;
	resolution.y = sizey;

	if (!Engine::GetRenderer()->_HasImplicitResizeHandling()) {
		Engine::GetRenderer()->HandleResize(resolution);
	}
}
