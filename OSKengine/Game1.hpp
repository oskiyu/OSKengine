#pragma once

#include "Game.h"

#include "Cube.h"
#include "ModelComponent.h"

#include "PlaceToken.h"
#include "SkyboxToken.h"

class Game1 : public Game {

public:

	void OnCreate() override {

	}

	void LoadContent() override {
		entityComponentSystem->RegisterGameObjectClass<Cube>("Cube");
		entityComponentSystem->RegisterGameObjectClass<PlayerCube>("PlayerCube");

		Fuente = content->LoadFont("Fonts/AGENCYB.ttf", 20);
		ShowFont = content->LoadFont("Fonts/AGENCYB.ttf", 40);

		renderSystem3D->GetRenderScene()->lights.directional.color = OSK::Color(0.9f, 0.9f, 0.8f);

		//INPUT SYSTEM
		inputSystem->RegisterAxisInputEvent("PlayerMoveX");
		inputSystem->RegisterAxisInputEvent("PlayerMoveY");

		inputSystem->GetAxisInputEvent("PlayerMoveX").linkedAxes.push_back(OSK::GamepadAxis::LEFT_X);
		inputSystem->GetAxisInputEvent("PlayerMoveX").linkedGamepadAxes.push_back({ OSK::Key::D, OSK::Key::A });

		inputSystem->GetAxisInputEvent("PlayerMoveY").linkedAxes.push_back(OSK::GamepadAxis::LEFT_Y);
		inputSystem->GetAxisInputEvent("PlayerMoveY").linkedGamepadAxes.push_back({ OSK::Key::S, OSK::Key::W });

		inputSystem->RegisterOneTimeInputEvent("Exit");
		inputSystem->GetOneTimeInputEvent("Exit").linkedKeys.push_back(OSK::Key::ESCAPE);
		inputSystem->RegisterOneTimeInputEvent("Fullscreen");
		inputSystem->GetOneTimeInputEvent("Fullscreen").linkedKeys.push_back(OSK::Key::F11);
		inputSystem->RegisterOneTimeInputEvent("ChangeVSync");
		inputSystem->GetOneTimeInputEvent("ChangeVSync").linkedKeys.push_back(OSK::Key::F1);
		inputSystem->RegisterOneTimeInputEvent("ChangeFXAA");
		inputSystem->GetOneTimeInputEvent("ChangeFXAA").linkedKeys.push_back(OSK::Key::F2);

		OSK::InputComponent input;

		input.GetAxisInputFunction("PlayerMoveX") = [this](deltaTime_t deltaTime, float axis) {
			const float sensitivity = 20 * deltaTime;
			GetRenderer()->defaultCamera3D.GetTransform()->AddPosition(GetRenderer()->defaultCamera3D.GetRightVector() * sensitivity * axis);
		};
		input.GetAxisInputFunction("PlayerMoveY") = [this](deltaTime_t deltaTime, float axis) {
			const float sensitivity = 20 * deltaTime;
			GetRenderer()->defaultCamera3D.GetTransform()->AddPosition(GetRenderer()->defaultCamera3D.GetFrontVector() * -sensitivity * axis);
		};
		
		input.GetOneTimeInputFunction("Exit") = [this]() {
			Exit();
		};

		input.GetOneTimeInputFunction("Fullscreen") = [this]() {
			GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
		};
		input.GetOneTimeInputFunction("ChangeVSync") = [this]() {
			if (GetRenderer()->GetCurrentPresentMode() == OSK::PresentMode::VSYNC)
				GetRenderer()->SetPresentMode(OSK::PresentMode::INMEDIATE);
			else
				GetRenderer()->SetPresentMode(OSK::PresentMode::VSYNC);
		};
		input.GetOneTimeInputFunction("ChangeFXAA") = [this]() {
			if (GetRenderer()->postProcessingSettings.useFxaa)
				GetRenderer()->postProcessingSettings.useFxaa = 0;
			else
				GetRenderer()->postProcessingSettings.useFxaa = 1;
		};

		ControlsObject = entityComponentSystem->Spawn<OSK::GameObject>();
		ControlsObject->AddComponent<OSK::InputComponent>(input);

		//ENTIDADES
		GetRenderer()->postProcessingSettings.useFxaa = 1;

		scene->Load("Levels/startLevel.sc");
			
		auto cube = scene->GetGameObjectByName<Cube>("Cube1");
		OSK::ModelComponent& model = cube->GetComponent<OSK::ModelComponent>();
		model.AddAnimatedModel("models/anim2/goblin2.dae", content);
		model.GetAnimatedModels()[0].material = GetRenderer()->GetMaterialSystem()->GetMaterial(GetRenderer()->defaultMaterial3D_Name)->CreateInstance();
		model.GetAnimatedModels()[0].material->SetBuffer(GetRenderer()->GetUniformBuffers());
		model.GetAnimatedModels()[0].material->SetDynamicBuffer(renderSystem3D->GetRenderScene()->bonesUbos);
		model.GetAnimatedModels()[0].material->SetBuffer(renderSystem3D->GetRenderScene()->shadowMap->dirShadowsUniformBuffers);
		model.GetAnimatedModels()[0].material->SetTexture(content->LoadTexture("models/anim2/td.png"));
		model.GetAnimatedModels()[0].material->SetBuffer(renderSystem3D->GetRenderScene()->lightsUniformBuffers);
		model.GetAnimatedModels()[0].material->SetTexture(content->LoadTexture("models/anim2/ts.png"));
		model.GetAnimatedModels()[0].material->SetTexture(renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.texture);
		model.GetAnimatedModels()[0].material->FlushUpdate();
		model.Link(cube->GetTransform());

		Player = scene->GetGameObjectByName<PlayerCube>("Player");
		Player->GetComponent<OSK::ModelComponent>().AddAnimatedModel("models/anim2/goblin2.dae", content);
		Player->GetComponent<OSK::ModelComponent>().GetAnimatedModels()[0].animationSpeed = 0.5f;
		OSK::ModelComponent& playerModel = Player->GetComponent<OSK::ModelComponent>();
		playerModel.GetAnimatedModels()[0].material = GetRenderer()->GetMaterialSystem()->GetMaterial(GetRenderer()->defaultMaterial3D_Name)->CreateInstance();
		playerModel.GetAnimatedModels()[0].material->SetBuffer(GetRenderer()->GetUniformBuffers());
		playerModel.GetAnimatedModels()[0].material->SetDynamicBuffer(renderSystem3D->GetRenderScene()->bonesUbos);
		playerModel.GetAnimatedModels()[0].material->SetBuffer(renderSystem3D->GetRenderScene()->shadowMap->dirShadowsUniformBuffers);
		playerModel.GetAnimatedModels()[0].material->SetTexture(content->LoadTexture("models/anim2/td.png"));
		playerModel.GetAnimatedModels()[0].material->SetBuffer(renderSystem3D->GetRenderScene()->lightsUniformBuffers);
		playerModel.GetAnimatedModels()[0].material->SetTexture(content->LoadTexture("models/anim2/ts.png"));
		playerModel.GetAnimatedModels()[0].material->SetTexture(renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.texture);
		playerModel.GetAnimatedModels()[0].material->FlushUpdate();
		playerModel.Link(Player->GetTransform());

		scene->GetGameObjectByName("Cube3")->Remove();
		scene->GetGameObjectByName("Cube2")->Remove();

		GetRenderer()->defaultCamera3D.GetTransform()->SetPosition(Player->GetTransform()->GetPosition());

		physicsSystem->terrainColissionType = OSK::PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY;
	}

	void OnTick(deltaTime_t deltaTime) override {
		GetRenderer()->defaultCamera3D.Girar(newMouseState.GetPosition().X - oldMouseState.GetPosition().X, -(newMouseState.GetPosition().Y - oldMouseState.GetPosition().Y));

		auto gamepad = GetWindow()->GetGamepadState(0);

		float sensitivity = 260.0f * deltaTime;
		float x = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_X) * sensitivity;
		float y = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_Y) * sensitivity;
		GetRenderer()->defaultCamera3D.Girar(x, -y);
	}

	void OnDraw2D() override {
		spriteBatch.Clear();

		spriteBatch.DrawSprite(GetRenderer()->OSKengineIconSprite);
		spriteBatch.DrawString(Fuente, "OSKengine " + std::string(OSK::ENGINE_VERSION), 1.0f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
		spriteBatch.DrawString(Fuente, "FPS " + std::to_string((int)GetFPS() - 1), 1.5f, OSK::Vector2(10), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::TOP_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
	}

	OSK::Font* Fuente = nullptr;
	OSK::Font* ShowFont = nullptr;

	OSK::GameObject* ControlsObject;

	PlayerCube* Player;

};
