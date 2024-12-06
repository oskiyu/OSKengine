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

// Shader passes
#include "ShadowsStaticPass.h"
#include "PbrResolvePass.h"
#include "BillboardGBufferPass.h"
#include "AnimatedGBufferPass.h"
#include "StaticGBufferPass.h"

#include "IteratorSystemExecutionJob.h"
#include "ConsumerSystemExecutionJob.h"

// Console commands
#include "GpuStatsCommand.h"

#include "AudioApiAl.h"

#include <GLFW/glfw3.h>
#include "FileIO.h"
#undef GetCurrentTime

using namespace OSK;
using namespace OSK::ECS;

UniquePtr<IO::Logger> Engine::logger;
UniquePtr<IO::IDisplay> Engine::display;
UniquePtr<GRAPHICS::IRenderer> Engine::renderer;
UniquePtr<ASSETS::AssetManager> Engine::assetManager;
UniquePtr<ECS::EntityComponentSystem> Engine::entityComponentSystem;
UniquePtr<IO::IUserInput> Engine::input;
UniquePtr<IO::InputManager> Engine::inputManager;
UniquePtr<AUDIO::IAudioApi> Engine::audioApi;
UniquePtr<JobSystem> Engine::m_jobSystem;
UniquePtr<UuidProvider> Engine::uuidProvider;
UniquePtr<ConsoleCommandExecutor> Engine::commandExecutor;
UIndex64 Engine::gameFrameIndex;

void Engine::Create(GRAPHICS::RenderApiType type) {
	logger = new IO::Logger;
	logger->Start("LOG_LAST.txt");

	display = new IO::Window;
	entityComponentSystem = new ECS::EntityComponentSystem(logger.GetPointer());
	input = new IO::PcUserInput;
	inputManager = new IO::InputManager;
	audioApi = new AUDIO::AudioApiAl;
	audioApi->Initialize();
	uuidProvider = new UuidProvider;
	m_jobSystem = new JobSystem;
	commandExecutor = new ConsoleCommandExecutor;

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
	m_jobSystem->WaitForAll();

	entityComponentSystem.Delete();
	assetManager.Delete();
	display.Delete();
	inputManager.Delete();

	renderer->Close();
	renderer.Delete();

	audioApi.Delete();
	logger.Delete();
	m_jobSystem.Delete();
}

void Engine::RegisterBuiltinAssets(GAME::DefaultContentProfile profile) {
	assetManager->RegisterLoader<ASSETS::TextureLoader>();
	assetManager->RegisterLoader<ASSETS::FontLoader>();
	assetManager->RegisterLoader<ASSETS::AudioLoader>();

	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_3D_ONLY) {
		assetManager->RegisterLoader<ASSETS::ModelLoader3D>();
		assetManager->RegisterLoader<ASSETS::CubemapTextureLoader>();
		assetManager->RegisterLoader<ASSETS::IrradianceMapLoader>();
		assetManager->RegisterLoader<ASSETS::SpecularMapLoader>();
		assetManager->RegisterLoader<ASSETS::PreBuiltColliderLoader>();
		assetManager->RegisterLoader<ASSETS::PreBuiltSplineLoader3D>();
	}

	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_2D_ONLY) {

	}
}

void Engine::RegisterBuiltinComponents(GAME::DefaultContentProfile profile) {
	entityComponentSystem->RegisterComponent<ECS::CameraComponent2D>();
	entityComponentSystem->RegisterComponent<ECS::Transform2D>();

	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_3D_ONLY) {
		entityComponentSystem->RegisterComponent<ECS::Transform3D>();
		entityComponentSystem->RegisterComponent<ECS::ModelComponent3D>();
		entityComponentSystem->RegisterComponent<ECS::CameraComponent3D>();
		entityComponentSystem->RegisterComponent<ECS::CollisionComponent>();
		entityComponentSystem->RegisterComponent<ECS::PhysicsComponent>();
	}

	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_2D_ONLY) {
		entityComponentSystem->RegisterComponent<GRAPHICS::Sprite>();
	}
}

void Engine::RegisterBuiltinSystems(GAME::DefaultContentProfile profile) {
	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_3D_ONLY) {
		ECS::SystemDependencies deferredRenderDependencies{};
		deferredRenderDependencies.executeAfterThese.insert(static_cast<std::string>(TransformApplierSystem::GetSystemName()));
		entityComponentSystem->RegisterSystem<ECS::DeferredRenderSystem>(deferredRenderDependencies);

		entityComponentSystem->RegisterSystem<ECS::PhysicsSystem>(SystemDependencies::Empty());

		ECS::SystemDependencies collisionDependencies{};
		collisionDependencies.executeAfterThese.insert(static_cast<std::string>(PhysicsSystem::GetSystemName()));
		collisionDependencies.exclusiveExecution = true;
		collisionDependencies.singleThreaded = true;
		entityComponentSystem->RegisterSystem<ECS::CollisionSystem>(collisionDependencies);

		ECS::SystemDependencies collisionResolverDependencies{};
		collisionResolverDependencies.executeAfterThese.insert(static_cast<std::string>(CollisionSystem::GetSystemName()));
		collisionResolverDependencies.exclusiveExecution = true;
		collisionResolverDependencies.singleThreaded = true;
		entityComponentSystem->RegisterSystem<ECS::PhysicsResolver>(collisionResolverDependencies);

		ECS::SystemDependencies transformApplierDependencies{};
		transformApplierDependencies.executeAfterThese.insert(static_cast<std::string>(PhysicsResolver::GetSystemName()));
		transformApplierDependencies.singleThreaded = true;
		entityComponentSystem->RegisterSystem<TransformApplierSystem>(transformApplierDependencies);

		entityComponentSystem->RegisterSystem<ECS::SkyboxRenderSystem>(SystemDependencies::Empty());
	}

	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_2D_ONLY) {
		entityComponentSystem->RegisterSystem<ECS::RenderSystem2D>(SystemDependencies::Empty());
	}
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

void Engine::RegisterBuiltinJobs() {

}

void Engine::RegisterBuiltinShaderPasses(GAME::DefaultContentProfile profile) {
	if (profile == GAME::DefaultContentProfile::ALL || profile == GAME::DefaultContentProfile::_3D_ONLY) {
		renderer->GetShaderPassFactory()->RegisterShaderPass<GRAPHICS::ShadowsStaticPass>();
		renderer->GetShaderPassFactory()->RegisterShaderPass<GRAPHICS::PbrResolverPass>();
		renderer->GetShaderPassFactory()->RegisterShaderPass<GRAPHICS::BillboardGBufferPass>();
		renderer->GetShaderPassFactory()->RegisterShaderPass<GRAPHICS::AnimatedGBufferPass>();
		renderer->GetShaderPassFactory()->RegisterShaderPass<GRAPHICS::StaticGBufferPass>();
	}
}

void Engine::RegisterBuiltinConsoleCommands() {
	commandExecutor->RegisterCommand<GpuStatsCommand>();
}


IO::Logger* Engine::GetLogger() {
	return logger.GetPointer();
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

JobSystem* Engine::GetJobSystem() {
	return m_jobSystem.GetPointer();
}

UuidProvider* Engine::GetUuidProvider() {
	return uuidProvider.GetPointer();
}

ConsoleCommandExecutor* Engine::GetCommandExecutor() {
	return commandExecutor.GetPointer();
}

float Engine::GetCurrentTime() {
	return static_cast<float>(glfwGetTime());
}

Version Engine::GetVersion() {
	static Version version{ 0, 0, 0 };

	return version;
}

std::string_view Engine::GetBuild() {
	return "2024.09.27a";
}

UIndex64 Engine::GetCurrentGameFrameIndex() {
	return gameFrameIndex;
}

void Engine::Update() {
	gameFrameIndex = (gameFrameIndex + 1) % std::numeric_limits<UIndex64>::max();
}
