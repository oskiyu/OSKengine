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
#include "GraphicsPipelineOgl.h"
#include "GpuVertexBufferOgl.h"
#include "GpuIndexBufferOgl.h"
#include "Vertex.h"
#include "Viewport.h"
#include "RenderApiType.h"
#include "MaterialSystem.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void GLAPIENTRY DebugConsole(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (type >= GL_DEBUG_TYPE_ERROR)
		Engine::GetLogger()->Log(IO::LogLevel::L_ERROR, message);
}

RendererOgl::RendererOgl() : IRenderer(RenderApiType::OPENGL) {

}

void RendererOgl::Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) {
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

	renderTargetsCamera = new ECS::CameraComponent2D;
	renderTargetsCamera->LinkToWindow(&window);
	renderTargetsCameraTransform.SetScale({ window.GetWindowSize().X / 2.0f, window.GetWindowSize().Y / 2.0f });
	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	isOpen = true;
}

OwnedPtr<IMaterialSlot> RendererOgl::_CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const {
	OSK_ASSERT(false, "No implementado.");
	
	return nullptr;
}

OwnedPtr<IGraphicsPipeline> RendererOgl::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, Format format, const VertexInfo& vertexInfo) {
	GraphicsPipelineOgl* pipeline = new GraphicsPipelineOgl();

	pipeline->Create(layout, currentGpu.GetPointer(), pipelineInfo, format, vertexInfo);

	return pipeline;
}

void RendererOgl::Close() {
	isOpen = false;
}

void RendererOgl::HandleResize() {
	IRenderer::HandleResize();
}

void RendererOgl::CreateSwapchain(PresentMode mode) {

}

void RendererOgl::CreateCommandQueues() {
	commandList = new CommandListOgl;
}

void RendererOgl::SubmitSingleUseCommandList(ICommandList* commandList) {

}

void RendererOgl::CreateSyncDevice() {

}

void RendererOgl::CreateGpuMemoryAllocator() {
	//gpuMemoryAllocator = new GpuMemoryAllocatorOgl(currentGpu.GetPointer());
}

OwnedPtr<IRaytracingPipeline> RendererOgl::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexTypeName) {
	OSK_ASSERT(false, "No implementado.");
	return nullptr;
}

void RendererOgl::PresentFrame() {
	//commandList->EndRenderpass(finalRenderpass.GetPointer());
	commandList->Close();

	commandList->Reset();
	commandList->Start();

	//commandList->BeginAndClearRenderpass(finalRenderpass.GetPointer(), Color::RED());

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
}

bool RendererOgl::SupportsRaytracing() const {
	return false;
}
