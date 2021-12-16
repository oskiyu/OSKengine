#include "Game.h"

#include "Components.h"

#include "GameObject.h"

void Game::SetupWindow() {
	window = new OSK::WindowAPI();
	window->SetWindow(windowCreateInfo.SizeX, windowCreateInfo.SizeY, windowCreateInfo.Name);
	window->SetMouseMovementMode(windowCreateInfo.MouseAcceleration);
	window->SetMouseMode(windowCreateInfo.MouseMode);
}

void Game::SetupRenderer() {
	renderer = new OSK::RenderAPI();

	renderer->window = window.GetPointer();
	renderer->SetPresentMode(rendererCreateInfo.VSyncMode);
	renderer->fpsLimit = rendererCreateInfo.FPSlimit;
	renderer->renderResolutionMultiplier = rendererCreateInfo.RendererResolution;
	renderer->Init(rendererCreateInfo.GameName, rendererCreateInfo.GameVersion);
}

void Game::Run() {
	OnCreate();

	Init();

	MainLoop();

	OnExit();

	Close();
}

OSK::RenderAPI* Game::GetRenderer() {
	return renderer.GetPointer();
}
OSK::WindowAPI* Game::GetWindow() {
	return window.GetPointer();
}
OSK::AudioSystem* Game::GetAudioSystem() {
	return audio.GetPointer();
}
uint32_t Game::GetFPS() const {
	return framerate;
}

void Game::Init() {
	SetupWindow();
	SetupRenderer();

	content = renderer->content.GetPointer();

	SetupSystems();

	spriteBatch = GetRenderer()->CreateSpriteBatch();
	spriteBatch.SetCamera(GetRenderer()->GetDefaultCamera2D());
	renderSystem3D->renderStage.AddSpriteBatch(&spriteBatch);

	entityComponentSystem->RegisterGameObjectClass<OSK::GameObject>();

	scene = new OSK::Scene(entityComponentSystem.GetPointer(), renderSystem3D.GetPointer());

	LoadContent();

	physicsSystem->terrain = renderSystem3D->renderScene->terreno.GetPointer();

	renderer->OskEngineIconSprite.transform.SetPosition({ 5.0f });
	renderer->OskEngineIconSprite.transform.SetScale({ 48.f });
}

void Game::SetupSystems() {
	entityComponentSystem = new OSK::EntityComponentSystem();

	entityComponentSystem->RegisterComponent<OSK::PhysicsComponent>();
	entityComponentSystem->RegisterComponent<OSK::InputComponent>();
	entityComponentSystem->RegisterComponent<OSK::ModelComponent>();
	entityComponentSystem->RegisterComponent<OSK::OnTickComponent>();
	entityComponentSystem->RegisterComponent<OSK::CollisionComponent>();

	physicsSystem = entityComponentSystem->RegisterSystem<OSK::PhysicsSystem>();
	inputSystem = entityComponentSystem->RegisterSystem<OSK::InputSystem>();
	renderSystem3D = entityComponentSystem->RegisterSystem<OSK::RenderSystem3D>();
	onTickSystem = entityComponentSystem->RegisterSystem<OSK::OnTickSystem>();
	audio = entityComponentSystem->RegisterSystem<OSK::AudioSystem>();

	audio->SetCamera3D(renderer->GetDefaultCamera());

	inputSystem->SetWindow(window.GetPointer());
	renderSystem3D->renderer = renderer.GetPointer();
	renderSystem3D->Init();
}

void Game::Close() {
	scene.Delete();
	entityComponentSystem.Delete();

	renderer.Delete();
}

void Game::Exit() {
	window->Close();
}

void Game::MainLoop() {
	deltaTime_t deltaTime = 1.0f;

	while (!window->WindowShouldClose()) {
		const deltaTime_t startTime = window->GetTime();
		window->PollEvents();
		window->UpdateKeyboardState(newKeyboardState);
		window->UpdateMouseState(newMouseState);

		//Update
		entityComponentSystem->OnTick(deltaTime);
		
		OnTick(deltaTime);

		//FPS
		framerateDeltaTime += deltaTime;
		framerateCount++;
		if (framerateDeltaTime > 1.0f) {
			framerate = framerateCount;
			framerateCount = 0;
			framerateDeltaTime = 0.0f;
		}

		//Draw
		OnDraw2D();

		renderer->RenderFrame();

		//End
		oldKeyboardState = newKeyboardState;
		oldMouseState = newMouseState;

		const deltaTime_t endTime = window->GetTime();
		deltaTime = endTime - startTime;
	}

	Close();
}
