#include "Game.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Vertex2D.h"
#include "Vertex.h"
#include "IGpuMemoryAllocator.h"
#include "Sprite.h"
#include "InputManager.h"

#include "GameExceptions.h"

#include "FileIO.h"
#include <json.hpp>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IGame::IGame(GAME::DefaultContentProfile defaultContentProfile) : m_defaultContentProfile(defaultContentProfile) {

}

void IGame::RegisterAssets() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterComponents() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterSystems() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterConsoleCommands() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnCreate() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick_BeforeEcs(TDeltaTime) {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick_AfterEcs(TDeltaTime) {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnExit() {
	// Sobreescrito en clase Game del juego.
}

void IGame::_Run() {
	nlohmann::json engineConfig = nlohmann::json::parse(FileIO::ReadFromFile("engine_config.json"));
	const std::string graphicsApi = engineConfig["graphics_backend"];

	if (graphicsApi == "DX12")
		Engine::Create(RenderApiType::DX12);
	else if (graphicsApi == "VULKAN")
		Engine::Create(GRAPHICS::RenderApiType::VULKAN);
	else {
		Engine::Create(GRAPHICS::RenderApiType::VULKAN);
	}

	CreateWindow();
	Engine::RegisterBuiltinVertices();
	SetupEngine();

	Engine::RegisterBuiltinAssets(m_defaultContentProfile);
	Engine::RegisterBuiltinComponents(m_defaultContentProfile);
	Engine::RegisterBuiltinSystems(m_defaultContentProfile);
	Engine::RegisterBuiltinEvents();
	Engine::RegisterBuiltinJobs();
	Engine::RegisterBuiltinShaderPasses(m_defaultContentProfile);
	Engine::RegisterBuiltinConsoleCommands();

	m_rootUiElement = new UI::FreeContainer(Engine::GetDisplay()->GetResolution().ToVector2f());


	DynamicArray<Vertex2D> vertices2d = {
		{ { 0, 0 }, { 0, 0 } },
		{ { 0, 1 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 0 } },
		{ { 1, 1 }, { 1, 1 } }
	};

	DynamicArray<TIndexSize> indices2d = {
		0, 1, 2, 1, 2, 3
	};

	Sprite::globalVertexBuffer = Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices2d, Vertex2D::GetVertexInfo(), GpuQueueType::MAIN).GetPointer();
	Sprite::globalIndexBuffer = Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices2d, GpuQueueType::MAIN).GetPointer();

	OSK_ASSERT(Engine::GetDisplay()->IsOpen(), WindowNotCreatedException());
	// OSK_ASSERT(Engine::GetRenderer()->IsOpen(), RenderedNotCreatedException());

	RegisterAssets();
	RegisterComponents();
	RegisterSystems();
	RegisterConsoleCommands();

	OnCreate();

	Engine::GetRenderer()->PresentFrame();
	while (Engine::GetDisplay()->IsOpen()) {
		Engine::Update();

		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetDisplay()->Update();

		Engine::GetInput()->Update();
		Engine::GetInputManager()->_Update(*Engine::GetInput());

		OnTick_BeforeEcs(m_deltaTime);
		Engine::GetEcs()->OnTick(m_deltaTime);
		OnTick_AfterEcs(m_deltaTime);

		Engine::GetEcs()->OnRender(Engine::GetRenderer()->GetMainCommandList());

		BuildFrame();

		Engine::GetEcs()->_ClearEventQueues();

		Engine::GetRenderer()->PresentFrame();

		HandleResizeEvents();
		UpdateFps(m_deltaTime);

		const TDeltaTime endTime = Engine::GetCurrentTime();
		m_deltaTime = endTime - startTime;
	}

	m_rootUiElement.Delete();

	Engine::GetRenderer()->WaitForCompletion();

	OnExit();

	Engine::Close();
}

void IGame::UpdateFps(TDeltaTime deltaTime) {
	m_framerateCountTimer += deltaTime;
	m_frameCount++;
	if (m_framerateCountTimer > 1.0f) {
		m_currentFps = m_frameCount;
		m_frameCount = 0;
		m_framerateCountTimer = 0.0f;
	}
}

void IGame::Exit() {
	Engine::GetDisplay()->Close();
}

void IGame::OnWindowResize(const Vector2ui& size) {
	// Sobreescrito en clase Game del juego.
}

USize32 IGame::GetFps() const {
	return m_currentFps;
}

const UI::IContainer& IGame::GetRootUiElement() const {
	return *m_rootUiElement.GetPointer();
}

UI::IContainer& IGame::GetRootUiElement() {
	return *m_rootUiElement.GetPointer();
}

void IGame::HandleResizeEvents() {
	const auto& resizeEvents = Engine::GetEcs()->GetEventQueue<IDisplay::ResolutionChangedEvent>();

	for (const auto& event : resizeEvents) {
		m_rootUiElement->SetSize(event.newResolution.ToVector2f());
		m_rootUiElement->Rebuild();

		OnWindowResize(event.newResolution);
	}
}
