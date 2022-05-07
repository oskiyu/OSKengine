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

using namespace OSK;

void IGame::OnCreate() {

}

void IGame::OnTick(TDeltaTime deltaTime) {

}

void IGame::OnPreRender() {

}

void IGame::OnPostRender() {

}

void IGame::OnExit() {

}

void IGame::Run() {
	Engine::Create(GRAPHICS::RenderApiType::DX12);

	CreateWindow();
	SetupEngine();

	OSK::DynamicArray<OSK::GRAPHICS::Vertex2D> vertices2d = {
		{ { 0, 0 }, { 0, 0 } },
		{ { 0, 1 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 0 } },
		{ { 1, 1 }, { 1, 1 } }
	};

	OSK::DynamicArray<OSK::GRAPHICS::TIndexSize> indices2d = {
		0, 1, 2, 1, 2, 3
	};

	OSK::GRAPHICS::Sprite::globalVertexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices2d).GetPointer();
	OSK::GRAPHICS::Sprite::globalIndexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices2d).GetPointer();

	OSK_ASSERT(Engine::GetWindow()->IsOpen(), "No se ha creado correctamente la ventana en CreateWindow().");
	OSK_ASSERT(Engine::GetRenderer()->IsOpen(), "No se ha inicializado correctamente el renderizador en SetupEngine().");

	OnCreate();

	Engine::GetRenderer()->PresentFrame();
	while (!Engine::GetWindow()->ShouldClose()) {
		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetWindow()->Update();

		Engine::GetEntityComponentSystem()->OnTick(deltaTime);
		OnTick(deltaTime);
		OnPreRender();

		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->Render(
			Engine::GetRenderer()->GetCommandList()
		);
		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem2D>()->Render(
			Engine::GetRenderer()->GetCommandList()
		);

		OnPostRender();
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

TSize IGame::GetFps() const {
	return currentFps;
}
