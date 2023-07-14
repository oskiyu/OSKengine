#include "Game.h"

#include "OSKengine.h"
#include "Window.h"
#include "EntityComponentSystem.h"
#include "RenderSystem3D.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Assert.h"
#include "RenderSystem2D.h"
#include "Vertex2D.h"
#include "Vertex.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryAllocatorVk.h"
#include "GpuBuffer.h"
#include "Sprite.h"
#include "InputManager.h"

#include "GameExceptions.h"

#include "FileIO.h"
#include <json.hpp>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void IGame::RegisterAssets() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterComponents() {
	// Sobreescrito en clase Game del juego.
}

void IGame::RegisterSystems() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnCreate() {
	// Sobreescrito en clase Game del juego.
}

void IGame::OnTick(TDeltaTime) {
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

	Engine::RegisterBuiltinAssets();
	Engine::RegisterBuiltinComponents();
	Engine::RegisterBuiltinSystems();
	Engine::RegisterBuiltinEvents();

	rootUiElement = new UI::FreeContainer(Engine::GetDisplay()->GetResolution().ToVector2f());


	DynamicArray<Vertex2D> vertices2d = {
		{ { 0, 0 }, { 0, 0 } },
		{ { 0, 1 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 0 } },
		{ { 1, 1 }, { 1, 1 } }
	};

	DynamicArray<TIndexSize> indices2d = {
		0, 1, 2, 1, 2, 3
	};

	Sprite::globalVertexBuffer = Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices2d, Vertex2D::GetVertexInfo()).GetPointer();
	Sprite::globalIndexBuffer = Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices2d).GetPointer();

	OSK_ASSERT(Engine::GetDisplay()->IsOpen(), WindowNotCreatedException());
	OSK_ASSERT(Engine::GetRenderer()->IsOpen(), RenderedNotCreatedException());

	RegisterAssets();
	RegisterComponents();
	RegisterSystems();

	OnCreate();

	Engine::GetRenderer()->PresentFrame();
	while (Engine::GetDisplay()->IsOpen()) {
		Engine::Update();

		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetDisplay()->Update();

		Engine::GetInput()->Update();
		Engine::GetInputManager()->_Update(*Engine::GetInput());

		Engine::GetEcs()->OnTick(deltaTime);
		OnTick(deltaTime);

		for (auto i : Engine::GetEcs()->GetRenderSystems())
			if (i->IsActive())
				i->Render(Engine::GetRenderer()->GetGraphicsCommandList());

		BuildFrame();

		Engine::GetEcs()->_ClearEventQueues();

		Engine::GetRenderer()->PresentFrame();

		HandleResizeEvents();
		UpdateFps(deltaTime);

		const TDeltaTime endTime = Engine::GetCurrentTime();
		deltaTime = endTime - startTime;
	}

	rootUiElement.Delete();

	Engine::GetRenderer()->WaitForCompletion();

	OnExit();

	Engine::Close();
}

void IGame::UpdateFps(TDeltaTime deltaTime) {
	framerateCountTimer += deltaTime;
	frameCount++;
	if (framerateCountTimer > 1.0f) {
		currentFps = frameCount;
		frameCount = 0;
		framerateCountTimer = 0.0f;
	}
}

void IGame::Exit() {
	Engine::GetDisplay()->Close();
}

void IGame::OnWindowResize(const Vector2ui& size) {
	// Sobreescrito en clase Game del juego.
}

USize32 IGame::GetFps() const {
	return currentFps;
}

const UI::IContainer& IGame::GetRootUiElement() const {
	return *rootUiElement.GetPointer();
}

UI::IContainer& IGame::GetRootUiElement() {
	return *rootUiElement.GetPointer();
}

void IGame::HandleResizeEvents() {
	const auto& resizeEvents = Engine::GetEcs()->GetEventQueue<IDisplay::ResolutionChangedEvent>();

	for (const auto& event : resizeEvents) {
		rootUiElement->SetSize(event.newResolution.ToVector2f());
		rootUiElement->Rebuild();

		OnWindowResize(event.newResolution);
	}
}
