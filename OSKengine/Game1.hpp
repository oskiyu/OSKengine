#pragma once

#include "Game.h"

#include "Cube.h"
#include "ModelComponent.h"

class Game1 : public Game {

public:

	void OnCreate() override {
		//RendererCreateInfo.RendererResolution = 2.0f;
	}

	void LoadContent() override {
		Fuente = Content->LoadFont("Fonts/AGENCYB.ttf", 20);
		ShowFont = Content->LoadFont("Fonts/AGENCYB.ttf", 40);

		RenderSystem3D->RScene->LoadSkybox("skybox/sky");
		RenderSystem3D->RScene->LoadHeightmap("heightmaps/Heightmap.png", { 5.0f }, 35);
		RenderSystem3D->RScene->Lights.Directional.Color = OSK::Color(0.9f, 0.9f, 0.8f);

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
		InputSystem->RegisterOneTimeInputEvent("ChangeFXAA");
		InputSystem->GetOneTimeInputEvent("ChangeFXAA").LinkedKeys.push_back(OSK::Key::F2);

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
		input.GetOneTimeInputFunction("ChangeFXAA") = [this]() {
			if (GetRenderer()->PostProcessingSettings.UseFXAA)
				GetRenderer()->PostProcessingSettings.UseFXAA = 0;
			else
				GetRenderer()->PostProcessingSettings.UseFXAA = 1;
		};

		ControlsObject.Spawn(ECS.GetPointer());
		ControlsObject.AddComponent<OSK::InputComponent>(input);

		//ENTIDADES
		Cubes.push_back({});
		Cubes.back().Spawn(ECS.GetPointer());
		OSK::ModelComponent& model = Cubes.back().GetComponent<OSK::ModelComponent>();
		model.AddAnimatedModel("models/anim2/goblin2.dae", Content);
		model.AnimatedModels[0].ModelTransform.SetScale({ 0.000025f });
		model.AnimatedModels[0].ModelMaterial = GetRenderer()->GetMaterialSystem()->GetMaterial(GetRenderer()->DefaultMaterial3D_Name)->CreateInstance();
		model.AnimatedModels[0].ModelMaterial->SetBuffer(GetRenderer()->GetUniformBuffers());
		model.AnimatedModels[0].ModelMaterial->SetDynamicBuffer(RenderSystem3D->RScene->BonesUBOs);
		model.AnimatedModels[0].ModelMaterial->SetBuffer(RenderSystem3D->RScene->shadowMap->DirShadowsUniformBuffers);
		model.AnimatedModels[0].ModelMaterial->SetTexture(Content->LoadTexture("models/anim2/td.png"));
		model.AnimatedModels[0].ModelMaterial->SetBuffer(RenderSystem3D->RScene->LightsUniformBuffers);
		model.AnimatedModels[0].ModelMaterial->SetTexture(Content->LoadTexture("models/anim2/ts.png"));
		model.AnimatedModels[0].ModelMaterial->SetTexture(RenderSystem3D->RScene->shadowMap->DirShadows->RenderedSprite.Texture2D);
		model.AnimatedModels[0].ModelMaterial->FlushUpdate();
		
		model.Link(&Cubes.back().Transform3D);

		Player.Spawn(ECS.GetPointer());
		Player.GetComponent<OSK::ModelComponent>().AddAnimatedModel("models/anim2/goblin2.dae", Content);
		Player.GetComponent<OSK::ModelComponent>().AnimatedModels[0].ModelTransform.SetScale({ 0.000025f });
		Player.GetComponent<OSK::ModelComponent>().AnimatedModels[0].AnimationSpeed = 0.5f;
		OSK::ModelComponent& playerModel = Player.GetComponent<OSK::ModelComponent>();
		playerModel.AnimatedModels[0].ModelMaterial = GetRenderer()->GetMaterialSystem()->GetMaterial(GetRenderer()->DefaultMaterial3D_Name)->CreateInstance();
		playerModel.AnimatedModels[0].ModelMaterial->SetBuffer(GetRenderer()->GetUniformBuffers());
		playerModel.AnimatedModels[0].ModelMaterial->SetDynamicBuffer(RenderSystem3D->RScene->BonesUBOs);
		playerModel.AnimatedModels[0].ModelMaterial->SetBuffer(RenderSystem3D->RScene->shadowMap->DirShadowsUniformBuffers);
		playerModel.AnimatedModels[0].ModelMaterial->SetTexture(Content->LoadTexture("models/anim2/td.png"));
		playerModel.AnimatedModels[0].ModelMaterial->SetBuffer(RenderSystem3D->RScene->LightsUniformBuffers);
		playerModel.AnimatedModels[0].ModelMaterial->SetTexture(Content->LoadTexture("models/anim2/ts.png"));
		playerModel.AnimatedModels[0].ModelMaterial->SetTexture(RenderSystem3D->RScene->shadowMap->DirShadows->RenderedSprite.Texture2D);
		playerModel.AnimatedModels[0].ModelMaterial->FlushUpdate();
		playerModel.Link(&Player.Transform3D);
		Player.Transform3D.AddPosition({ 40, 0, 0 });

		GetRenderer()->DefaultCamera3D.CameraTransform.SetPosition(Player.Transform3D.GlobalPosition);
		GetRenderer()->PostProcessingSettings.UseFXAA = 1;

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
		SpriteBatch.DrawString(Fuente, "OSKengine " + std::string(OSK::ENGINE_VERSION), 1.0f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
	}

	OSK::Font* Fuente = nullptr;
	OSK::Font* ShowFont = nullptr;

	OSK::GameObject ControlsObject;

	std::vector<Cube> Cubes;
	PlayerCube Player;

};
