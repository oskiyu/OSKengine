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
#include "VertexOgl.h"
#include "GraphicsPipelineOgl.h"
#include "GpuVertexBufferOgl.h"
#include "Vertex.h"
#include "Viewport.h"

using namespace OSK;

GraphicsPipelineOgl* pipeline = nullptr;
GpuVertexBufferOgl* vertexBuffer = nullptr;

void GLAPIENTRY DebugConsole(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (type >= GL_DEBUG_TYPE_ERROR)
		Engine::GetLogger()->Log(LogLevel::L_ERROR, message);
}

void RendererOgl::Initialize(const std::string& appName, const Version& version, const Window& window) {
	this->window = &window;

	auto gladResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	OSK_ASSERT(gladResult != 0, "No se ha podido cargar OpenGL.");

	glViewport(0, 0, window.GetWindowSize().X, window.GetWindowSize().Y);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugConsole, 0);

	Engine::GetLogger()->InfoLog("Iniciado OpenGL.");

	CreateCommandQueues();
	CreateGpuMemoryAllocator();
	CreateMainRenderpass();

	PipelineCreateInfo info{};
	info.vertexPath = "./Resources/Shaders/VK/shader.vert";
	info.fragmentPath = "./Resources/Shaders/VK/shader.frag";
	info.polygonMode = PolygonMode::FILL;
	info.cullMode = PolygonCullMode::NONE;
	info.frontFaceType = PolygonFrontFaceType::CLOCKWISE;

	pipeline = new GraphicsPipelineOgl();
	pipeline->Create(currentGpu.GetPointer(), info);

	DynamicArray<Vertex3D> vertices = {
		{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ {0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
	};

	vertexBuffer = gpuMemoryAllocator->CreateVertexBuffer(vertices).GetPointer()->As<GpuVertexBufferOgl>();
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

	Vector4ui windowRec = {
		0,
		0,
		window->GetWindowSize().X,
		window->GetWindowSize().Y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);

	commandList->BindPipeline(pipeline);
	commandList->BindVertexBuffer(vertexBuffer);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}
