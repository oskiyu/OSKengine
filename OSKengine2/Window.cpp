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

using namespace OSK;
using namespace OSK::IO;

Window::Window() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

Window::~Window() {
	glfwTerminate();
}


void Window::Create(Vector2ui size, const std::string& title) {

	// Creación de la ventana.
	window = glfwCreateWindow(size.X, size.Y, title.c_str(), NULL, NULL);
	OSK_ASSERT(window.HasValue(), "No se ha podido iniciar la ventana.");
	glfwMakeContextCurrent(window.GetPointer());

	resolution = size;

	// Obtención de variables de glfw.
	glfwSetWindowUserPointer(window.GetPointer(), this);
	monitor = glfwGetPrimaryMonitor();
	monitorInfo = glfwGetVideoMode(monitor.GetPointer());

	refreshRate = monitorInfo->refreshRate;

	// Establece los callbacks.
	glfwSetFramebufferSizeCallback(window.GetPointer(), Window::GlfwResizeCallback);

	{
		int x = 0;
		int y = 0;
		int nmChannels = 0;
		stbi_uc* icon = stbi_load("Resources/Icons/engineIcon.png", &x, &y, &nmChannels, 4);

		GLFWimage glfwIcon[1]{};
		glfwIcon[0].width = x;
		glfwIcon[0].height = y;
		glfwIcon[0].pixels = icon;

		glfwSetWindowIcon(window.GetPointer(), _countof(glfwIcon), glfwIcon);

		stbi_image_free(icon);
	}

	isOpen = true;

	reinterpret_cast<PcUserInput*>(Engine::GetInput())->Initialize(*this);
}

void Window::QueryInterface(TInterfaceUuid uuid, void** ptr) const {
	if (uuid == OSK_IUUID(IFullscreenableDisplay))
		*ptr = (IFullscreenableDisplay*)this;
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
		glfwGetWindowPos(window.GetPointer(), &previous.position.X, &previous.position.Y);
		glfwGetWindowSize(window.GetPointer(), &previous.size.X, &previous.size.Y);

		glfwSetWindowMonitor(window.GetPointer(), monitor.GetPointer(), 0, 0, monitorInfo->width, monitorInfo->height, monitorInfo->refreshRate);

		resolution.X = monitorInfo->width;
		resolution.Y = monitorInfo->height;
	}
	else {
		glfwSetWindowMonitor(window.GetPointer(), nullptr, previous.position.X, previous.position.Y, previous.size.X, previous.size.Y, monitorInfo->refreshRate);
	}

	isFullscreen = fullscreen;
}

void Window::Close() {
	glfwSetWindowShouldClose(window.GetPointer(), true);
	isOpen = false;
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

void Window::GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey) {
	GetWindowForCallback(window)->ResizeCallback(sizex, sizey);
}

void Window::ResizeCallback(int sizex, int sizey) {
	resolution.X = sizex;
	resolution.Y = sizey;

	if (!Engine::GetRenderer()->_HasImplicitResizeHandling())
		Engine::GetRenderer()->HandleResize();
}
