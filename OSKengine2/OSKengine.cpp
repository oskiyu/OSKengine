#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "RendererOgl.h"
#include "RendererDx12.h"
#include "RendererVulkan.h"
#include "RenderApiType.h"
#include "Assert.h"

using namespace OSK;

UniquePtr<Logger> Engine::logger;
UniquePtr<Window> Engine::window;
UniquePtr<IRenderer> Engine::renderer;

void Engine::Create(RenderApiType type) {
	logger = new Logger;
	window = new Window;

	logger->Start("LOG_LAST.txt");
	window->SetRenderApiType(type);

	logger->InfoLog("Iniciando OSKengine.");
	logger->InfoLog("	Version: " 
		+ std::to_string(GetVersion().mayor) + "."
		+ std::to_string(GetVersion().menor) + "."
		+ std::to_string(GetVersion().parche) + ".");
	logger->InfoLog("	Build " + GetBuild() + ".");

	switch (type) {
	case OSK::RenderApiType::OPENGL:
		renderer = new RendererOgl;

		break;

	case OSK::RenderApiType::VULKAN:
		renderer = new RendererVulkan;

		break;

	case OSK::RenderApiType::DX12:
		renderer = new RendererDx12;

		break;

	case OSK::RenderApiType::OPENGL_ES:
		OSK_ASSERT(false, "El renderizador de OPENGL_ES no está implementado.");

		break;
	}
}

void Engine::Close() {
	renderer.Delete();
	window.Delete();
	logger.Delete();
}

Logger* Engine::GetLogger() {
	return logger.GetPointer();
}

Window* Engine::GetWindow() {
	return window.GetPointer();
}

IRenderer* Engine::GetRenderer() {
	return renderer.GetPointer();
}

const Version& Engine::GetVersion() {
	static Version version{0, 0, 0};

	return version;
}

const std::string& Engine::GetBuild() {
	static std::string build = "2021.11.15a";

	return build;
}
