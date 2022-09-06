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
#include "IGpuVertexBuffer.h"
#include "Sprite.h"
#include "IGpuIndexBuffer.h"
#include "InputManager.h"

#include "FileIO.h"
#include <json.hpp>

using namespace OSK;

void IGame::RegisterAssets() {

}

void IGame::RegisterComponents() {

}

void IGame::RegisterSystems() {

}

void IGame::OnCreate() {

}

void IGame::OnTick(TDeltaTime deltaTime) {

}

void IGame::OnExit() {

}

void IGame::Run() {
	nlohmann::json engineConfig = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile("engine_config.json"));
	const std::string graphicsApi = engineConfig["graphics_backend"];

	if (graphicsApi == "DX12")
		Engine::Create(GRAPHICS::RenderApiType::DX12);
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

	OSK::DynamicArray<OSK::GRAPHICS::Vertex2D> vertices2d = {
		{ { 0, 0 }, { 0, 0 } },
		{ { 0, 1 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 0 } },
		{ { 1, 1 }, { 1, 1 } }
	};

	OSK::DynamicArray<OSK::GRAPHICS::TIndexSize> indices2d = {
		0, 1, 2, 1, 2, 3
	};

	OSK::GRAPHICS::Sprite::globalVertexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices2d, OSK::GRAPHICS::Vertex2D::GetVertexInfo()).GetPointer();
	OSK::GRAPHICS::Sprite::globalIndexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices2d).GetPointer();

	OSK_ASSERT(Engine::GetWindow()->IsOpen(), "No se ha creado correctamente la ventana en CreateWindow().");
	OSK_ASSERT(Engine::GetRenderer()->IsOpen(), "No se ha inicializado correctamente el renderizador en SetupEngine().");

	RegisterAssets();
	RegisterComponents();
	RegisterSystems();

	Engine::GetRenderer()->AddResizeCallback([this](const Vector2ui& size) { this->OnWindowResize(size); });

	OnCreate();

	Engine::GetRenderer()->PresentFrame();
	while (!Engine::GetWindow()->ShouldClose()) {
		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetWindow()->Update();

		Engine::GetInputManager()->_Update(*Engine::GetWindow());

		Engine::GetEntityComponentSystem()->OnTick(deltaTime);
		OnTick(deltaTime);
		for (auto i : Engine::GetEntityComponentSystem()->GetRenderSystems())
			i->Render(Engine::GetRenderer()->GetGraphicsCommandList());

		BuildFrame();

		Engine::GetRenderer()->PresentFrame();

		Engine::GetWindow()->UpdateMouseAndKeyboardOldStates();

		framerateCountTimer += deltaTime;
		frameCount++;
		if (framerateCountTimer > 1.0f) {
			currentFps = frameCount;
			frameCount = 0;
			framerateCountTimer = 0.0f;
		}

		const TDeltaTime endTime = Engine::GetCurrentTime();
		deltaTime = endTime - startTime;
	}

	OnExit();

	Engine::Close();
}

void IGame::Exit() {
	Engine::GetWindow()->Close();
}

void IGame::OnWindowResize(const Vector2ui& size) {

}

TSize IGame::GetFps() const {
	return currentFps;
}
