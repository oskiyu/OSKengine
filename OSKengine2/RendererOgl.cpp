#include "RendererOgl.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"
#include "Window.h"
#include "Version.h"

using namespace OSK;

void RendererOgl::Initialize(const std::string& appName, const Version& version, const Window& window) {
	auto gladResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	OSK_ASSERT(gladResult != 0, "No se ha podido cargar OpenGL.");

	glViewport(0, 0, window.GetWindowSize().X, window.GetWindowSize().Y);

	Engine::GetLogger()->InfoLog("Iniciado OpenGL.");
}

void RendererOgl::Close() {
	Engine::GetLogger()->InfoLog("Destruyendo el renderizador de OpenGL.");
}

void RendererOgl::CreateSwapchain() {

}

void RendererOgl::CreateCommandQueues() {

}

void RendererOgl::PresentFrame() {

}

void RendererOgl::CreateSyncDevice() {

}

void RendererOgl::CreateGpuMemoryAllocator() {

}
