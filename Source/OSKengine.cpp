#include "OSKengine.h"

#include "Logger.h"

#include "Platforms.h"
#ifdef OSK_ANDROID
#include "AndroidDisplay.h"
#include "AndroidInput.h"
#include <android_native_app_glue.h>
#include "AndroidLogger.h"
#include <android/log.h>
#else
#include "Window.h"
#endif

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

#include "TransformComponent3D.h"
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

// GDR
#include "GdrGeometryComponent.h"
#include "GdrTexturedComponent.h"
#include "GdrMaterialComponent.h"

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

#ifdef OSK_PC_ANY
#include <GLFW/glfw3.h>
#endif

#include "FileIO.h"
#undef GetCurrentTime

using namespace OSK;
using namespace OSK::ECS;

UniquePtr<IO::ILogger> Engine::logger;
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

#ifdef OSK_ANDROID
void Engine::Create(GRAPHICS::RenderApiType type, struct android_app* app) {
#else
void Engine::Create(GRAPHICS::RenderApiType type) {
#endif

#ifdef OSK_ANDROID
	logger = MakeUnique<IO::AndroidLogger>();
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s %i", "Establecido el Logger ", logger.GetPointer());

	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s %i", "android_app*: ", (USize64)app);
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s %u", "android_app->window: ", (USize64)app->window);
#else
	logger = MakeUnique<IO::Logger>();
#endif
	logger->Start("LOG_LAST.txt");

#ifdef OSK_ANDROID
	display = MakeUnique<IO::AndroidWindow>(app);
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s", "Establecido el display.");
#else
	display = MakeUnique<IO::Window>();
#endif

	entityComponentSystem = MakeUnique<ECS::EntityComponentSystem>(logger.GetPointer());

#ifdef OSK_PC_ANY
	input = MakeUnique<IO::PcUserInput>();
#endif
#ifdef OSK_ANDROID
	input = MakeUnique<IO::AndroidInput>(app);
	__android_log_print(ANDROID_LOG_DEBUG, "OSKENGINE", "%s", "Establecido el input.");
#endif
	inputManager = MakeUnique<IO::InputManager>();
	audioApi = MakeUnique<AUDIO::AudioApiAl>();
	audioApi->Initialize();
	uuidProvider = MakeUnique<UuidProvider>();
	m_jobSystem = MakeUnique<JobSystem>();
	commandExecutor = MakeUnique<ConsoleCommandExecutor>();

	logger->InfoLog("Iniciando OSKengine.");
	logger->InfoLog(std::format("\tVersion: {}.{}.{}", 
		GetVersion().mayor,
		GetVersion().menor,
		GetVersion().parche));
	logger->InfoLog(std::format("\tBuild {}.",GetBuild()));

	const nlohmann::json engineConfig = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile("engine_config.json"));
	const bool requestRayTracing = engineConfig.contains("use_rt") && engineConfig["use_rt"] == 1;

	switch (type) {

#ifdef OSK_USE_VULKAN_BACKEND
	case OSK::GRAPHICS::RenderApiType::VULKAN:
		renderer = MakeUnique<GRAPHICS::RendererVk>(requestRayTracing);
		break;
#endif
	}

	assetManager = MakeUnique<ASSETS::AssetManager>();
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
		entityComponentSystem->RegisterComponent<ECS::TransformComponent3D>();
		entityComponentSystem->RegisterComponent<ECS::ModelComponent3D>();
		entityComponentSystem->RegisterComponent<ECS::CameraComponent3D>();
		entityComponentSystem->RegisterComponent<ECS::CollisionComponent>();
		entityComponentSystem->RegisterComponent<ECS::PhysicsComponent>();

		// GDR
		entityComponentSystem->RegisterComponent<ECS::GdrGeometryComponent>();
		entityComponentSystem->RegisterComponent<ECS::GdrTexturedComponent>();
		entityComponentSystem->RegisterComponent<ECS::GdrMaterialComponent>();
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
	renderer->GetMaterialSystem()->RegisterVertexType<GRAPHICS::EmptyVertex>();
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


IO::ILogger* Engine::GetLogger() {
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
#ifdef OSK_PC_ANY
	return static_cast<float>(glfwGetTime());
#else
	static const auto start = std::chrono::high_resolution_clock::now();
	const auto now = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<float> diff = now - start;

	return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()) / 1000.0f;
#endif
}

Version Engine::GetVersion() {
	static Version version{ 0, 0, 0 };

	return version;
}

std::string_view Engine::GetBuild() {
	return "2025.05.06a";
}

UIndex64 Engine::GetCurrentGameFrameIndex() {
	return gameFrameIndex;
}

void Engine::Update() {
	gameFrameIndex = (gameFrameIndex + 1) % std::numeric_limits<UIndex64>::max();
}

