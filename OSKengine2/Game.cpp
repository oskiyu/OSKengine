#include "Game.h"

#include "OSKengine.h"
#include "Window.h"
#include "EntityComponentSystem.h"
#include "RenderSystem3D.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Assert.h"

using namespace OSK;

void IGame::OnCreate() {

}

void IGame::OnTick(TDeltaTime deltaTime) {

}

void IGame::OnExit() {

}

void IGame::Run() {
	Engine::Create(GRAPHICS::RenderApiType::DX12);

	CreateWindow();
	SetupEngine();

	OSK_ASSERT(Engine::GetWindow()->IsOpen(), "No se ha creado correctamente la ventana en CreateWindow().");
	OSK_ASSERT(Engine::GetRenderer()->IsOpen(), "No se ha inicializado correctamente el renderizador en SetupEngine().");

	OnCreate();

	Engine::GetRenderer()->PresentFrame();
	while (!Engine::GetWindow()->ShouldClose()) {
		const TDeltaTime startTime = Engine::GetCurrentTime();

		Engine::GetWindow()->Update();

		Engine::GetEntityComponentSystem()->OnTick(deltaTime);
		OnTick(deltaTime);

		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->Render(
			Engine::GetRenderer()->GetCommandList()
		);
		Engine::GetRenderer()->PresentFrame();

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

	Engine::Close();
	OnExit();
}

void IGame::Exit() {
	Engine::GetWindow()->Close();
}

TSize IGame::GetFps() const {
	return currentFps;
}
