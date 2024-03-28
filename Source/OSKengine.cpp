#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "RendererDx12.h"
#include "RendererVk.h"
#include "RenderApiType.h"
#include "Assert.h"

#include "AssetManager.h"
#include "TextureLoader.h"
#include "ModelLoader3D.h"
#include "CubemapTextureLoader.h"
#include "Font.h"
#include "FontLoader.h"
#include "SpecularMapLoader.h"
#include "AudioAsset.h"
#include "AudioLoader.h"
#include "IrradianceMapLoader.h"
#include "PreBuiltColliderLoader.h"
#include "PreBuiltSplineLoader3D.h"
#include "TransformApplierSystem.h"

#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "RenderSystem3D.h"
#include "CameraComponent3D.h"
#include "EntityComponentSystem.h"
#include "Vertex2D.h"
#include "Vertex3D.h"
#include "MaterialSystem.h"
#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "GdrDeferredSystem.h"
#include "IGpuMemoryAllocator.h"
#include "CameraComponent2D.h"
#include "Sprite.h"
#include "TerrainComponent.h"
#include "TerrainRenderSystem.h"
#include "InputManager.h"
#include "DeferredRenderSystem.h"
#include "SkyboxRenderSystem.h"
#include "PcUserInput.h"
#include "CollisionComponent.h"
#include "CollisionEvent.h"
#include "CollisionSystem.h"
#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "PhysicsResolver.h"

#include "TreeNormalsRenderSystem.h"

#include "AudioApiAl.h"

#include <GLFW/glfw3.h>
#include "FileIO.h"
#undef GetCurrentTime

using namespace OSK;
using namespace OSK::ECS;

UniquePtr<IO::Logger> Engine::logger;
UniquePtr<IO::Console> Engine::console;
UniquePtr<IO::IDisplay> Engine::display;
UniquePtr<GRAPHICS::IRenderer> Engine::renderer;
UniquePtr<ASSETS::AssetManager> Engine::assetManager;
UniquePtr<ECS::EntityComponentSystem> Engine::entityComponentSystem;
UniquePtr<IO::IUserInput> Engine::input;
UniquePtr<IO::InputManager> Engine::inputManager;
UniquePtr<AUDIO::IAudioApi> Engine::audioApi;
UniquePtr<UuidProvider> Engine::uuidProvider;
UIndex64 Engine::gameFrameIndex;

void Engine::Create(GRAPHICS::RenderApiType type) {
	logger = new IO::Logger;
	logger->Start("LOG_LAST.txt");

	console = new IO::Console;

	display = new IO::Window;
	entityComponentSystem = new ECS::EntityComponentSystem(logger.GetPointer());
	input = new IO::PcUserInput;
	inputManager = new IO::InputManager;
	audioApi = new AUDIO::AudioApiAl;
	audioApi->Initialize();
	uuidProvider = new UuidProvider;

	logger->InfoLog("Iniciando OSKengine.");
	logger->InfoLog(std::format("\tVersion: {}.{}.{}", 
		GetVersion().mayor,
		GetVersion().menor,
		GetVersion().parche));
	logger->InfoLog(std::format("\tBuild {}.",GetBuild()));

	const nlohmann::json engineConfig = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile("engine_config.json"));
	const bool requestRayTracing = engineConfig.contains("use_rt") && engineConfig["use_rt"] == 1;

	switch (type) {

	case OSK::GRAPHICS::RenderApiType::VULKAN:
		renderer = new GRAPHICS::RendererVk(requestRayTracing);

		break;

	case OSK::GRAPHICS::RenderApiType::DX12:
		renderer = new GRAPHICS::RendererDx12(requestRayTracing);

		break;
	}

	assetManager = new ASSETS::AssetManager();
}

void Engine::Close() {
	renderer->WaitForCompletion();

	entityComponentSystem.Delete();
	assetManager.Delete();
	display.Delete();
	inputManager.Delete();
	renderer.Delete();
	audioApi.Delete();
	logger.Delete();
}

void Engine::RegisterBuiltinAssets() {
	assetManager->RegisterLoader<ASSETS::TextureLoader>();
	assetManager->RegisterLoader<ASSETS::ModelLoader3D>();
	assetManager->RegisterLoader<ASSETS::CubemapTextureLoader>();
	assetManager->RegisterLoader<ASSETS::FontLoader>();
	assetManager->RegisterLoader<ASSETS::IrradianceMapLoader>();
	assetManager->RegisterLoader<ASSETS::SpecularMapLoader>();
	assetManager->RegisterLoader<ASSETS::AudioLoader>();
	assetManager->RegisterLoader<ASSETS::PreBuiltColliderLoader>();
	assetManager->RegisterLoader<ASSETS::PreBuiltSplineLoader3D>();
}

void Engine::RegisterBuiltinComponents() {
	entityComponentSystem->RegisterComponent<ECS::Transform3D>();
	entityComponentSystem->RegisterComponent<ECS::ModelComponent3D>();
	entityComponentSystem->RegisterComponent<ECS::CameraComponent3D>();
	entityComponentSystem->RegisterComponent<GRAPHICS::Sprite>();
	entityComponentSystem->RegisterComponent<ECS::CameraComponent2D>();
	entityComponentSystem->RegisterComponent<ECS::Transform2D>();
	entityComponentSystem->RegisterComponent<ECS::TerrainComponent>();
	entityComponentSystem->RegisterComponent<ECS::CollisionComponent>();
	entityComponentSystem->RegisterComponent<ECS::PhysicsComponent>();
}

void Engine::RegisterBuiltinSystems() {
#ifdef OSK_USE_FORWARD_RENDERER
	entityComponentSystem->RegisterSystem<ECS::RenderSystem3D>(ECS::ISystem::DEFAULT_EXECUTION_ORDER);
#elif defined(OSK_USE_DEFERRED_RENDERER)
	entityComponentSystem->RegisterSystem<ECS::TreeNormalsRenderSystem>(ECS::SystemDependencies::Empty());

	ECS::SystemDependencies deferredRenderDependencies{};
	deferredRenderDependencies.executeAfterThese.insert(static_cast<std::string>(ECS::TreeNormalsRenderSystem::GetSystemName()));
	deferredRenderDependencies.executeAfterThese.insert(static_cast<std::string>(TransformApplierSystem::GetSystemName()));
	entityComponentSystem->RegisterSystem<ECS::DeferredRenderSystem>(deferredRenderDependencies);
#elif defined(OSK_USE_HYBRID_RENDERER)
	entityComponentSystem->RegisterSystem<ECS::HybridRenderSystem>();
#elif defined(OSK_USE_GDR_RENDERER)
	entityComponentSystem->RegisterSystem<ECS::TreeNormalsRenderSystem>(ECS::ISystem::DEFAULT_EXECUTION_ORDER - 1);
	entityComponentSystem->RegisterSystem<ECS::GdrDeferredRenderSystem>(ECS::ISystem::DEFAULT_EXECUTION_ORDER);
#elif OSK_NO_DEFAULT_RENDERER
#else 
#error No hay un renderizador por defecto
#endif

	entityComponentSystem->RegisterSystem<ECS::PhysicsSystem>(SystemDependencies::Empty());

	ECS::SystemDependencies collisionDependencies{};
	collisionDependencies.executeAfterThese.insert(static_cast<std::string>(PhysicsSystem::GetSystemName()));
	entityComponentSystem->RegisterSystem<ECS::CollisionSystem>(collisionDependencies);

	ECS::SystemDependencies collisionResolverDependencies{};
	collisionResolverDependencies.executeAfterThese.insert(static_cast<std::string>(CollisionSystem::GetSystemName()));
	entityComponentSystem->RegisterSystem<ECS::PhysicsResolver>(collisionResolverDependencies);

	ECS::SystemDependencies transformApplierDependencies{};
	transformApplierDependencies.executeAfterThese.insert(static_cast<std::string>(PhysicsResolver::GetSystemName()));
	entityComponentSystem->RegisterSystem<TransformApplierSystem>(transformApplierDependencies);

	entityComponentSystem->RegisterSystem<ECS::SkyboxRenderSystem>(SystemDependencies::Empty());
	entityComponentSystem->RegisterSystem<ECS::RenderSystem2D>(SystemDependencies::Empty());
}

void Engine::RegisterBuiltinEvents() {
	entityComponentSystem->RegisterEventType<ECS::CollisionEvent>();
	entityComponentSystem->RegisterEventType<IO::IDisplay::ResolutionChangedEvent>();
}

void Engine::RegisterBuiltinVertices() {
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::Vertex2D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::Vertex3D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::PositionOnlyVertex3D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::VertexAnim3D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::VertexCollisionDebug3D>();
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::GdrVertex3D>();
}


IO::Logger* Engine::GetLogger() {
	return logger.GetPointer();
}

IO::Console* Engine::GetConsole() {
	return console.GetPointer();
}

IO::IDisplay* Engine::GetDisplay() {
	return display.GetPointer();
}

GRAPHICS::IRenderer* Engine::GetRenderer() {
	return renderer.GetPointer();
}

ASSETS::AssetManager* Engine::GetAssetManager() {
	return assetManager.GetPointer();
}

ECS::EntityComponentSystem* Engine::GetEcs() {
	return entityComponentSystem.GetPointer();
}

IO::IUserInput* Engine::GetInput() {
	return input.GetPointer();
}

IO::InputManager* Engine::GetInputManager() {
	return inputManager.GetPointer();
}

AUDIO::IAudioApi* Engine::GetAudioApi() {
	return audioApi.GetPointer();
}

UuidProvider* Engine::GetUuidProvider() {
	return uuidProvider.GetPointer();
}


float Engine::GetCurrentTime() {
	return static_cast<float>(glfwGetTime());
}

Version Engine::GetVersion() {
	static Version version{ 0, 0, 0 };

	return version;
}

std::string_view Engine::GetBuild() {
	return "2024.03.28a";
}

UIndex64 Engine::GetCurrentGameFrameIndex() {
	return gameFrameIndex;
}

void Engine::Update() {
	gameFrameIndex = (gameFrameIndex + 1) % std::numeric_limits<UIndex64>::max();
}
