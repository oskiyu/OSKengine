#include "Game.h"

#include "Components.h"

void Game::SetupWindow() {
	window = new OSK::WindowAPI();
	window->SetWindow(WindowCreateInfo.SizeX, WindowCreateInfo.SizeY, WindowCreateInfo.Name);
	window->SetMouseMovementMode(WindowCreateInfo.MouseAcceleration);
	window->SetMouseMode(WindowCreateInfo.MouseMode);
}

void Game::SetupRenderer() {
	renderer = new OSK::RenderAPI();

	renderer->Window = window;
	renderer->SetPresentMode(RendererCreateInfo.VSyncMode);
	renderer->FPSlimit = RendererCreateInfo.FPSlimit;
	renderer->Init(RendererCreateInfo.GameName, RendererCreateInfo.GameVersion);
}

void Game::Run() {
	OnCreate();

	Init();

	MainLoop();

	OnExit();

	Close();
}

OSK::RenderAPI* Game::GetRenderer() {
	return renderer;
}
OSK::WindowAPI* Game::GetWindow() {
	return window;
}
OSK::AudioSystem* Game::GetAudioSystem() {
	return audio;
}
deltaTime_t Game::GetFPS() {
	return Framerate;
}

void Game::Init() {
	SetupWindow();
	SetupRenderer();

	Content = renderer->Content;

	SetupSystems();

	LoadContent();

	PhysicsSystem->FloorTerrain = RenderSystem3D->RScene->Terreno;

	renderer->OSKengineIconSprite.SpriteTransform.SetPosition({ 5.0f });
	renderer->OSKengineIconSprite.SpriteTransform.SetScale({ 48.f });
}

void Game::SetupSystems() {
	ECS = new OSK::EntityComponentSystem();

	ECS->RegisterComponent<OSK::PhysicsComponent>();
	ECS->RegisterComponent<OSK::InputComponent>();
	ECS->RegisterComponent<OSK::ModelComponent>();
	ECS->RegisterComponent<OSK::OnTickComponent>();
	ECS->RegisterComponent<OSK::CollisionComponent>();

	PhysicsSystem = ECS->RegisterSystem<OSK::PhysicsScene>();
	InputSystem = ECS->RegisterSystem<OSK::InputSystem>();
	RenderSystem3D = ECS->RegisterSystem<OSK::RenderSystem3D>();
	OnTickSystem = ECS->RegisterSystem<OSK::OnTickSystem>();

	InputSystem->SetWindow(window);
	RenderSystem3D->Renderer = renderer;
	RenderSystem3D->Init();
}

void Game::Close() {
	delete renderer;
	delete window;
	delete audio;

	delete ECS;
}

void Game::Exit() {
	window->Close();
}

void Game::MainLoop() {
	deltaTime_t deltaTime = 1.0f;

	while (!window->WindowShouldClose()) {
		deltaTime_t startTime = window->GetTime();
		window->PollEvents();
		window->UpdateKeyboardState(NewKS);
		window->UpdateMouseState(NewMS);

		//Update
		ECS->OnTick(deltaTime);

		OnTick(deltaTime);

		//FPS
		FramerateDeltaTime += deltaTime;
		FramerateCount++;
		if (FramerateDeltaTime > 1.0f) {
			Framerate = FramerateCount;
			FramerateCount = 0;
			FramerateDeltaTime = 0.0f;
		}

		//Draw
		renderer->DefaultCamera3D.updateVectors();

		OnDraw2D();

		renderer->RenderFrame();

		//End
		OldKS = NewKS;
		OldMS = NewMS;

		deltaTime_t endTime = window->GetTime();
		deltaTime = endTime - startTime;
	}

	Close();
}
