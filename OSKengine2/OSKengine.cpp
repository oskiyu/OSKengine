#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "RendererDx12.h"
#include "RendererVulkan.h"
#include "RenderApiType.h"
#include "Assert.h"
#include "AssetManager.h"
#include "TextureLoader.h"
#include "ModelLoader3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "RenderSystem3D.h"
#include "CameraComponent3D.h"
#include "EntityComponentSystem.h"
#include "CubemapTextureLoader.h"
#include "Vertex2D.h"
#include "Vertex3D.h"
#include "MaterialSystem.h"
#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "IGpuMemoryAllocator.h"
#include "CameraComponent2D.h"
#include "Sprite.h"
#include "Transform2D.h"
#include "Font.h"
#include "FontLoader.h"
#include "TerrainComponent.h"
#include "TerrainRenderSystem.h"
#include "InputManager.h"
#include "IrradianceMapLoader.h"
#include "PbrDeferredRenderSystem.h"
#include "SkyboxRenderSystem.h"

#include <GLFW/glfw3.h>
#include "FileIO.h"
#undef GetCurrentTime

using namespace OSK;

UniquePtr<IO::Logger> Engine::logger;
UniquePtr<IO::Window> Engine::window;
UniquePtr<GRAPHICS::IRenderer> Engine::renderer;
UniquePtr<ASSETS::AssetManager> Engine::assetManager;
UniquePtr<ECS::EntityComponentSystem> Engine::entityComponentSystem;
UniquePtr<IO::InputManager> Engine::inputManager;

void Engine::Create(GRAPHICS::RenderApiType type) {
	logger = new IO::Logger;
	window = new IO::Window;
	entityComponentSystem = new ECS::EntityComponentSystem;
	inputManager = new IO::InputManager;

	logger->Start("LOG_LAST.txt");
	window->SetRenderApiType(type);

	logger->InfoLog("Iniciando OSKengine.");
	logger->InfoLog("	Version: " 
		+ std::to_string(GetVersion().mayor) + "."
		+ std::to_string(GetVersion().menor) + "."
		+ std::to_string(GetVersion().parche) + ".");
	logger->InfoLog("	Build " + GetBuild() + ".");

	nlohmann::json engineConfig = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile("engine_config.json"));
	const bool requestRayTracing = engineConfig.contains("use_rt") && engineConfig["use_rt"] == 1;

	switch (type) {

	case OSK::GRAPHICS::RenderApiType::VULKAN:
		renderer = new GRAPHICS::RendererVulkan(requestRayTracing);

		break;

	case OSK::GRAPHICS::RenderApiType::DX12:
		renderer = new GRAPHICS::RendererDx12(requestRayTracing);

		break;
	}

	assetManager = new ASSETS::AssetManager();
}

void Engine::Close() {
	entityComponentSystem.Delete();
	assetManager.Delete();
	renderer.Delete();
	window.Delete();
	logger.Delete();
	inputManager.Delete();
}

void Engine::RegisterBuiltinAssets() {
	assetManager->RegisterLoader<ASSETS::TextureLoader>();
	assetManager->RegisterLoader<ASSETS::ModelLoader3D>();
	assetManager->RegisterLoader<ASSETS::CubemapTextureLoader>();
	assetManager->RegisterLoader<ASSETS::FontLoader>();
	assetManager->RegisterLoader<ASSETS::IrradianceMapLoader>();
}

void Engine::RegisterBuiltinComponents() {
	entityComponentSystem->RegisterComponent<ECS::Transform3D>();
	entityComponentSystem->RegisterComponent<ECS::ModelComponent3D>();
	entityComponentSystem->RegisterComponent<ECS::CameraComponent3D>();
	entityComponentSystem->RegisterComponent<GRAPHICS::Sprite>();
	entityComponentSystem->RegisterComponent<ECS::CameraComponent2D>();
	entityComponentSystem->RegisterComponent<ECS::Transform2D>();
	entityComponentSystem->RegisterComponent<ECS::TerrainComponent>();
}

void Engine::RegisterBuiltinSystems() {
	entityComponentSystem->RegisterSystem<ECS::HybridRenderSystem>();
	entityComponentSystem->RegisterSystem<ECS::SkyboxRenderSystem>();
	entityComponentSystem->RegisterSystem<ECS::RenderSystem2D>();
	// entityComponentSystem->RegisterSystem<ECS::TerrainRenderSystem>();
}

void Engine::RegisterBuiltinVertices() {
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::Vertex2D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::Vertex3D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::VertexAnim3D>();
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

ECS::EntityComponentSystem* Engine::GetEcs() {
	return entityComponentSystem.GetPointer();
}

IO::InputManager* Engine::GetInputManager() {
	return inputManager.GetPointer();
}

float Engine::GetCurrentTime() {
	return static_cast<float>(glfwGetTime());
}

const Version& Engine::GetVersion() {
	static Version version{ 0, 0, 0 };

	return version;
}

const std::string& Engine::GetBuild() {
	static std::string build = "2022.11.02a";

	return build;
}
