#pragma once

#include "Game.h"

#include "Cube.h"
#include "ModelComponent.h"

class Game1 : public Game {

public:

	void OnCreate() override {
		
	}

	void LoadContent() override {
		SpriteBatch = GetRenderer()->CreateSpriteBatch();
		SpriteBatch.SetCamera(GetRenderer()->DefaultCamera2D);
		GetRenderer()->AddSpriteBatch(&SpriteBatch);

		Fuente = Content->LoadFont("Fonts/AGENCYB.ttf", 20);
		ShowFont = Content->LoadFont("Fonts/AGENCYB.ttf", 40);

		RenderSystem3D->RScene->LoadSkybox("skybox/skybox.ktx");
		RenderSystem3D->RScene->LoadHeightmap("heightmaps/Heightmap.png", { 5.0f }, 35);

		//INPUT SYSTEM
		InputSystem->RegisterInputEvent("PlayerMoveFront");
		InputSystem->GetInputEvent("PlayerMoveFront").LinkedKeys.push_back(OSK::Key::W);
		InputSystem->RegisterInputEvent("PlayerMoveBack");
		InputSystem->GetInputEvent("PlayerMoveBack").LinkedKeys.push_back(OSK::Key::S);
		InputSystem->RegisterInputEvent("PlayerMoveLeft");
		InputSystem->GetInputEvent("PlayerMoveLeft").LinkedKeys.push_back(OSK::Key::A);
		InputSystem->RegisterInputEvent("PlayerMoveRight");
		InputSystem->GetInputEvent("PlayerMoveRight").LinkedKeys.push_back(OSK::Key::D);

		InputSystem->RegisterInputEvent("MoveFront");
		InputSystem->GetInputEvent("MoveFront").LinkedKeys.push_back(OSK::Key::UP);
		InputSystem->RegisterInputEvent("MoveBack");
		InputSystem->GetInputEvent("MoveBack").LinkedKeys.push_back(OSK::Key::DOWN);
		InputSystem->RegisterInputEvent("MoveLeft");
		InputSystem->GetInputEvent("MoveLeft").LinkedKeys.push_back(OSK::Key::LEFT);
		InputSystem->RegisterInputEvent("MoveRight");
		InputSystem->GetInputEvent("MoveRight").LinkedKeys.push_back(OSK::Key::RIGHT);

		InputSystem->RegisterInputEvent("GirarX");
		InputSystem->GetInputEvent("GirarX").LinkedKeys.push_back(OSK::Key::X);
		InputSystem->RegisterInputEvent("GirarY");
		InputSystem->GetInputEvent("GirarY").LinkedKeys.push_back(OSK::Key::Z);

		InputSystem->RegisterOneTimeInputEvent("Exit");
		InputSystem->GetOneTimeInputEvent("Exit").LinkedKeys.push_back(OSK::Key::ESCAPE);
		InputSystem->RegisterOneTimeInputEvent("Fullscreen");
		InputSystem->GetOneTimeInputEvent("Fullscreen").LinkedKeys.push_back(OSK::Key::F11);
		InputSystem->RegisterOneTimeInputEvent("ChangeVSync");
		InputSystem->GetOneTimeInputEvent("ChangeVSync").LinkedKeys.push_back(OSK::Key::F1);

		OSK::InputComponent input;

		input.GetInputFunction("PlayerMoveFront") = [this](deltaTime_t deltaTime) {
			GetRenderer()->DefaultCamera3D.CameraTransform.AddPosition(GetRenderer()->DefaultCamera3D.Front * 3 * deltaTime);
		};
		input.GetInputFunction("PlayerMoveBack") = [this](deltaTime_t deltaTime) {
			GetRenderer()->DefaultCamera3D.CameraTransform.AddPosition(-GetRenderer()->DefaultCamera3D.Front * 3 * deltaTime);
		};
		input.GetInputFunction("PlayerMoveLeft") = [this](deltaTime_t deltaTime) {
			GetRenderer()->DefaultCamera3D.CameraTransform.AddPosition(-GetRenderer()->DefaultCamera3D.Right * 3 * deltaTime);
		};
		input.GetInputFunction("PlayerMoveRight") = [this](deltaTime_t deltaTime) {
			GetRenderer()->DefaultCamera3D.CameraTransform.AddPosition(GetRenderer()->DefaultCamera3D.Right * 3 * deltaTime);
		};
		input.GetOneTimeInputFunction("Exit") = [this]() {
			Exit();
		};

		input.GetOneTimeInputFunction("Fullscreen") = [this]() {
			GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen);
		};
		input.GetOneTimeInputFunction("ChangeVSync") = [this]() {
			if (GetRenderer()->GetCurrentPresentMode() == OSK::PresentMode::VSYNC)
				GetRenderer()->SetPresentMode(OSK::PresentMode::INMEDIATE);
			else
				GetRenderer()->SetPresentMode(OSK::PresentMode::VSYNC);
		};

		ControlsObject.Create(ECS);
		ControlsObject.AddComponent<OSK::InputComponent>(input);

		Cubes.push_back({});
		Cubes.back().Create(ECS);
		//Cubes.back().GetComponent<OSK::ModelComponent>().AddModel("models/cube/cube.obj", Content);
		Cubes.back().GetComponent<OSK::ModelComponent>().AddAnimatedModel("models/anim2/goblin2.dae", Content);
		Cubes.back().GetComponent<OSK::ModelComponent>().AnimatedModels[0]->ModelTransform.SetScale({ 0.0005f });
		Cubes.back().GetComponent<OSK::ModelComponent>().AnimatedModels[0]->ModelTransform.RotateWorldSpace(90.0f, { 1.0f, 0.0f, 0.0f });
		Cubes.back().GetComponent<OSK::ModelComponent>().Link(&Cubes.back().Transform3D);

		Player.Create(ECS);
		Player.GetComponent<OSK::ModelComponent>().AddAnimatedModel("models/anim2/goblin2.dae", Content);
		//Player.GetComponent<OSK::ModelComponent>().AddModel("models/cube/cube.obj", Content);
		Player.GetComponent<OSK::ModelComponent>().AnimatedModels[0]->ModelTransform.SetScale({ 0.0005f });
		Player.GetComponent<OSK::ModelComponent>().AnimatedModels[0]->ModelTransform.RotateWorldSpace(90.0f, { 1.0f, 0.0f, 0.0f });
		Player.GetComponent<OSK::ModelComponent>().AnimatedModels[0]->AnimationSpeed = 2.0f;
		Player.GetComponent<OSK::ModelComponent>().Link(&Player.Transform3D);
		Player.Transform3D.AddPosition({ 20, 0, 0 });

		PhysicsSystem->TerrainColissionType = OSK::PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY;
	}

	void OnTick(deltaTime_t deltaTime) override {
		mouseVar_t deltaX = NewMS.PositionX - OldMS.PositionX;
		mouseVar_t deltaY = NewMS.PositionY - OldMS.PositionY;
		GetRenderer()->DefaultCamera3D.Girar(deltaX, -deltaY);
	}

	void OnDraw2D() override {
		SpriteBatch.Clear();

		SpriteBatch.DrawSprite(GetRenderer()->OSKengineIconSprite);
	}

	OSK::SpriteBatch SpriteBatch;

	OSK::Font* Fuente = nullptr;
	OSK::Font* ShowFont = nullptr;

	OSK::GameObject ControlsObject;

	std::vector<Cube> Cubes;
	PlayerCube Player;

};
