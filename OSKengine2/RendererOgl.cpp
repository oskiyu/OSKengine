#include "RendererOgl.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"
#include "Window.h"
#include "Version.h"
#include "CommandListOgl.h"
#include "GpuMemoryAllocatorOgl.h"
#include "RenderpassOgl.h"
#include "RenderpassType.h"
#include "Color.hpp"

using namespace OSK;

void GLAPIENTRY DebugConsole(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (type >= GL_DEBUG_TYPE_ERROR)
		Engine::GetLogger()->Log(LogLevel::L_ERROR, message);
}

void RendererOgl::Initialize(const std::string& appName, const Version& version, const Window& window) {
	auto gladResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	OSK_ASSERT(gladResult != 0, "No se ha podido cargar OpenGL.");

	glViewport(0, 0, window.GetWindowSize().X, window.GetWindowSize().Y);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugConsole, 0);

	Engine::GetLogger()->InfoLog("Iniciado OpenGL.");

	CreateCommandQueues();
	CreateGpuMemoryAllocator();
	CreateMainRenderpass();
}

void RendererOgl::Close() {
	Engine::GetLogger()->InfoLog("Destruyendo el renderizador de OpenGL.");
}

void RendererOgl::CreateSwapchain() {

}

void RendererOgl::CreateCommandQueues() {
	commandList = new CommandListOgl;
}

void RendererOgl::SubmitSingleUseCommandList(ICommandList* commandList) {

}

void RendererOgl::CreateSyncDevice() {

}

void RendererOgl::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorOgl(currentGpu.GetPointer());
}

void RendererOgl::CreateMainRenderpass() {
	renderpass = new RenderpassOgl(RenderpassType::FINAL);
}

void RendererOgl::PresentFrame() {
	commandList->EndRenderpass(renderpass.GetPointer());
	commandList->Close();

	commandList->Reset();
	commandList->Start();

	commandList->BeginAndClearRenderpass(renderpass.GetPointer(), Color::RED());
}
