#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "RendererOgl.h"
#include "RendererDx12.h"
#include "RendererVulkan.h"
#include "RenderApiType.h"
#include "Assert.h"
#include "AssetManager.h"
#include "TextureLoader.h"
#include "ModelLoader3D.h"
#include "EntityComponentSystem.h"

#include <GLFW/glfw3.h>
#undef GetCurrentTime;

using namespace OSK;

UniquePtr<IO::Logger> Engine::logger;
UniquePtr<IO::Window> Engine::window;
UniquePtr<GRAPHICS::IRenderer> Engine::renderer;
UniquePtr<ASSETS::AssetManager> Engine::assetManager;
UniquePtr<ECS::EntityComponentSystem> Engine::entityComponentSystem;

void Engine::Create(GRAPHICS::RenderApiType type) {
	logger = new IO::Logger;
	window = new IO::Window;
	entityComponentSystem = new ECS::EntityComponentSystem;

	logger->Start("LOG_LAST.txt");
	window->SetRenderApiType(type);

	logger->InfoLog("Iniciando OSKengine.");
	logger->InfoLog("	Version: " 
		+ std::to_string(GetVersion().mayor) + "."
		+ std::to_string(GetVersion().menor) + "."
		+ std::to_string(GetVersion().parche) + ".");
	logger->InfoLog("	Build " + GetBuild() + ".");

	switch (type) {
	case GRAPHICS::RenderApiType::OPENGL:
		renderer = new GRAPHICS::RendererOgl;

		break;

	case OSK::GRAPHICS::RenderApiType::VULKAN:
		renderer = new GRAPHICS::RendererVulkan;

		break;

	case OSK::GRAPHICS::RenderApiType::DX12:
		renderer = new GRAPHICS::RendererDx12;

		break;

	case OSK::GRAPHICS::RenderApiType::OPENGL_ES:
		OSK_ASSERT(false, "El renderizador de OPENGL_ES no está implementado.");

		break;
	}

	assetManager = new ASSETS::AssetManager();
	assetManager->RegisterLoader<ASSETS::TextureLoader>();
	assetManager->RegisterLoader<ASSETS::ModelLoader3D>();
}

void Engine::Close() {
	assetManager.Delete();
	renderer.Delete();
	window.Delete();
	logger.Delete();
}

IO::Logger* Engine::GetLogger() {
	return logger.GetPointer();
}

IO::Window* Engine::GetWindow() {
	return window.GetPointer();
}

GRAPHICS::IRenderer* Engine::GetRenderer() {
	return renderer.GetPointer();
}

ASSETS::AssetManager* Engine::GetAssetManager() {
	return assetManager.GetPointer();
}

ECS::EntityComponentSystem* Engine::GetEntityComponentSystem() {
	return entityComponentSystem.GetPointer();
}

float Engine::GetCurrentTime() {
	return glfwGetTime();
}

const Version& Engine::GetVersion() {
	static Version version{0, 0, 0};

	return version;
}

const std::string& Engine::GetBuild() {
	static std::string build = "2022.03.06a";

	return build;
}
