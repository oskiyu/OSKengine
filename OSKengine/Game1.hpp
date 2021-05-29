#pragma once

#include "Game.h"

#include "Cube.h"
#include "ModelComponent.h"

#include "PlaceToken.h"
#include "SkyboxToken.h"

#include "UiFunctionality.h"

class Game1 : public Game {

public:

	void OnCreate() override {

	}

	void LoadContent() override {
		entityComponentSystem->RegisterGameObjectClass<Cube>("Cube");
		entityComponentSystem->RegisterGameObjectClass<PlayerCube>("PlayerCube");

		buttonTexture = content->LoadTexture("assets/game/ui/button.png", OSK::TextureFilterType::NEAREST);
		buttonTexture = content->DefaultTexture;

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
			GetWindow()->SetMouseMode((OSK::MouseInputMode::FREE));
		};

		input.GetOneTimeInputFunction("Fullscreen") = [this]() {
			GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
			GetRenderer()->defaultCamera2D.targetSize = GetWindow()->GetSize().ToVector2f();
			GetRenderer()->defaultCamera2D.Update();
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




		{
			userInterface = new OSK::UiElement();
			userInterface->layout = OSK::UiLayout::FIXED;
			userInterface->shrinkToChildren = false;
			userInterface->SetSize(GetWindow()->GetSize().ToVector2f());

			OSK::UiElement* topbar = new OSK::UiElement();
			topbar->layout = OSK::UiLayout::HORIZONTAL;
			topbar->SetPositionFromAnchor({ 5.0f });
			topbar->sprite.texture = buttonTexture;
			topbar->InitSprite(content, GetRenderer());
			topbar->originalSpireColor = OSK::Color(0.2f) * 0.6f;
			topbar->anchor = OSK::UiAnchor::TOP;

			OSK::UiElement* logo = new OSK::UiElement();
			logo->SetSize(48.0f);
			logo->sprite.texture = GetRenderer()->OSKengineIconSprite.texture;
			logo->InitSprite(content, GetRenderer());
			logo->marging = 0.0f;

			OSK::uiSetDraggableFunctionality(logo);

			OSK::UiElement* separator = new OSK::UiElement();
			separator->SetSize({ 48.0f, 5 });

			topbar->AddElement(logo);
			topbar->AddElement(separator);

			float buttonSize = 20.0f;
			float barSize = 20.0f;

			//Options
			OSK::UiElement* optionsMenu = new OSK::UiElement();
			{
				optionsMenu->SetSize({ 200.0f });
				optionsMenu->sprite.texture = buttonTexture;
				optionsMenu->InitSprite(content, GetRenderer());
				optionsMenu->originalSpireColor = OSK::Color(0.2f) * 0.6f;

				OSK::UiElement* header = new OSK::UiElement();
				header->SetSize({ 34.0f });
				header->fuente = Fuente;
				header->text = " Opciones";
				header->sprite.texture = buttonTexture;
				header->InitSprite(content, GetRenderer());
				header->originalSpireColor = OSK::Color(0.1f) * 0.9f;

				OSK::Vector2f newSize = { optionsMenu->GetSize().X - optionsMenu->marging.X * 2 - header->marging.X * 2, header->GetSize().Y };
				header->SetSize(newSize);

				barSize = header->GetSize().Y;

				OSK::uiSetDropdownFuncionality(header, true);

				OSK::UiElement* fxaaOption = new OSK::UiElement();
				{
					fxaaOption->layout = OSK::UiLayout::HORIZONTAL;
					OSK::UiElement* text = new OSK::UiElement();
					text->fuente = Fuente;
					text->text = "FXAA: ";
					text->SetSize(Fuente->GetTextSize("FXAA: ", { 1.0f }));

					OSK::UiElement* button = new OSK::UiElement();
					button->SetSize({ text->GetSize().X, buttonSize });
					button->sprite.texture = buttonTexture;
					button->InitSprite(content, GetRenderer());
					button->spriteColorOnHover = OSK::Color(0.5f);

					OSK::uiSetCheckboxFunctionality(button, &GetRenderer()->postProcessingSettings.useFxaa, { 0.0f, 1.0f, 0.0f }, OSK::Color::RED());

					fxaaOption->SetSize(text->GetSize());

					fxaaOption->AddElement(text);
					fxaaOption->AddElement(button);
				}

				OSK::UiElement* vsyncOption = new OSK::UiElement();
				{
					vsyncOption->layout = OSK::UiLayout::HORIZONTAL;
					OSK::UiElement* text = new OSK::UiElement();
					text->fuente = Fuente;
					text->text = "V-Sync: ";
					text->SetSize(Fuente->GetTextSize("V-Sync: ", { 1.0f }));

					OSK::UiElement* button = new OSK::UiElement();
					button->SetSize({ text->GetSize().X, buttonSize });
					button->sprite.texture = buttonTexture;
					button->InitSprite(content, GetRenderer());
					button->spriteColorOnHover = OSK::Color(0.5f);

					auto func = [this]() {
						if (GetRenderer()->GetCurrentPresentMode() != OSK::PresentMode::VSYNC) {
							GetRenderer()->SetPresentMode(OSK::PresentMode::VSYNC);

							return true;
						}

						GetRenderer()->SetPresentMode(OSK::PresentMode::INMEDIATE);
						return false;
					};
					OSK::uiSetCheckboxFunctionality(button, func, { 0.0f, 1.0f, 0.0f }, OSK::Color::RED());

					vsyncOption->SetSize(text->GetSize());

					vsyncOption->AddElement(text);
					vsyncOption->AddElement(button);
				}

				fxaaOption->isActive = false;
				vsyncOption->isActive = false;

				optionsMenu->AddElement(header);
				optionsMenu->AddElement(fxaaOption);
				optionsMenu->AddElement(vsyncOption);
			}
			OSK::UiElement* irAlJuego = new OSK::UiElement();
			{
				OSK::UiElement* header = new OSK::UiElement();
				header->sprite.texture = buttonTexture;
				header->InitSprite(content, GetRenderer());
				header->originalSpireColor = OSK::Color(0.1f) * 0.9f;
				header->SetSize({ optionsMenu->GetSize().X - optionsMenu->marging.X * 2 - header->marging.X * 2, optionsMenu->GetSize().Y });

				irAlJuego->SetSize(32);

				OSK::UiElement* button = new OSK::UiElement();
				irAlJuego->SetSize({ irAlJuego->GetSize().X * 1.5f, barSize });
				irAlJuego->sprite.texture = content->LoadTexture("assets/editor/play.png");
				irAlJuego->InitSprite(content, GetRenderer());

				float r = 15.0f / 255.0f;
				float g = 255.0f / 255.0f;
				float b = 91.0f / 255.0f;
				irAlJuego->originalSpireColor = OSK::Color::WHITE();
				irAlJuego->spriteColorOnHover = OSK::Color::WHITE() * 0.8f;

				irAlJuego->onClick = [this](OSK::UiElement* sender, const OSK::Vector2f& mousePos) {
					GetWindow()->SetMouseMode(OSK::MouseInputMode::ALWAYS_RETURN);
				};

				header->AddElement(irAlJuego);
				topbar->AddElement(header);
			}

			topbar->AddElement(optionsMenu);

			userInterface->AddElement(topbar);
		}

	}

	void OnTick(deltaTime_t deltaTime) override {
		GetRenderer()->defaultCamera3D.Girar(newMouseState.GetPosition().X - oldMouseState.GetPosition().X, -(newMouseState.GetPosition().Y - oldMouseState.GetPosition().Y));

		auto gamepad = GetWindow()->GetGamepadState(0);

		float sensitivity = 260.0f * deltaTime;
		float x = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_X) * sensitivity;
		float y = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_Y) * sensitivity;
		GetRenderer()->defaultCamera3D.Girar(x, -y);

		userInterface->SetSize(GetWindow()->GetSize().ToVector2f());
		userInterface->Update(newMouseState.GetMouseRectangle(), newMouseState.IsButtonDown(OSK::ButtonCode::BUTTON_LEFT));
	}

	void OnDraw2D() override {
		spriteBatch.Clear();

		spriteBatch.DrawString(Fuente, "OSKengine " + std::string(OSK::ENGINE_VERSION), 1.0f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
		spriteBatch.DrawString(Fuente, "FPS " + std::to_string((int)GetFPS() - 1), 1.5f, OSK::Vector2(10), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::TOP_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);

		userInterface->Draw(&spriteBatch);
	}

	OSK::Font* Fuente = nullptr;
	OSK::Font* ShowFont = nullptr;

	OSK::GameObject* ControlsObject;
	OSK::Texture* buttonTexture = nullptr;

	PlayerCube* Player;

	OSK::UiElement* userInterface = nullptr;

};
