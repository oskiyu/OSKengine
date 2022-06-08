#pragma once

#include "Game.h"

#include "Cube.h"
#include "ModelComponent.h"

#include "PlaceToken.h"
#include "SkyboxToken.h"

#include "UiFunctionality.h"
#include "MaterialSlot.h"


class Game1 : public Game {

public:

	void OnCreate() override {

	}

	void LoadContent() override {
		entityComponentSystem->RegisterGameObjectClass<Cube>("Cube");
		entityComponentSystem->RegisterGameObjectClass<PlayerCube>("PlayerCube");
		entityComponentSystem->RegisterGameObjectClass<Car>("Car");
		entityComponentSystem->RegisterGameObjectClass<Wheel>("Wheel");

		buttonTexture = content->LoadTexture("assets/game/ui/button.png", OSK::TextureFilterType::NEAREST);
		buttonTexture = content->DefaultTexture;

		Fuente = content->LoadFont("Fonts/AGENCYB.ttf", 20);
		ShowFont = content->LoadFont("Fonts/AGENCYB.ttf", 40);

		renderSystem3D->GetRenderScene()->lights.directional.color = OSK::Color(0.9f, 0.9f, 0.8f);

		//INPUT SYSTEM
		inputSystem->RegisterAxisInputEvent("Acelerar");
		inputSystem->GetAxisInputEvent("Acelerar").linkedAxes.push_back(OSK::GamepadAxis::R2);
		inputSystem->GetAxisInputEvent("Acelerar").linkedGamepadAxes.push_back({ OSK::Key::UP, OSK::Key::DOWN });

		inputSystem->RegisterAxisInputEvent("Frenar");
		inputSystem->GetAxisInputEvent("Frenar").linkedAxes.push_back(OSK::GamepadAxis::L2);

		inputSystem->RegisterAxisInputEvent("GirarL");
		inputSystem->GetAxisInputEvent("GirarL").linkedAxes.push_back(OSK::GamepadAxis::LEFT_X);
		inputSystem->GetAxisInputEvent("GirarL").linkedGamepadAxes.push_back({ OSK::Key::RIGHT, OSK::Key::LEFT });


		/*inputSystem->RegisterAxisInputEvent("PlayerMoveX");
		inputSystem->RegisterAxisInputEvent("PlayerMoveY");

		inputSystem->GetAxisInputEvent("PlayerMoveX").linkedAxes.push_back(OSK::GamepadAxis::LEFT_X);
		inputSystem->GetAxisInputEvent("PlayerMoveX").linkedGamepadAxes.push_back({ OSK::Key::D, OSK::Key::A });

		inputSystem->GetAxisInputEvent("PlayerMoveY").linkedAxes.push_back(OSK::GamepadAxis::LEFT_Y);
		inputSystem->GetAxisInputEvent("PlayerMoveY").linkedGamepadAxes.push_back({ OSK::Key::S, OSK::Key::W });*/

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
			GetRenderer()->GetDefaultCamera()->GetTransform()->AddPosition(GetRenderer()->GetDefaultCamera()->GetTransform()->GetRightVector() * sensitivity * axis);
		};
		input.GetAxisInputFunction("PlayerMoveY") = [this](deltaTime_t deltaTime, float axis) {
			const float sensitivity = 20 * deltaTime;
			GetRenderer()->GetDefaultCamera()->GetTransform()->AddPosition(GetRenderer()->GetDefaultCamera()->GetTransform()->GetForwardVector() * -sensitivity * axis);
		};
		
		input.GetOneTimeInputFunction("Exit") = [this]() {
			GetWindow()->SetMouseMode((OSK::MouseInputMode::FREE));
		};

		input.GetOneTimeInputFunction("Fullscreen") = [this]() {
			GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
			GetRenderer()->GetDefaultCamera2D()->targetSize = GetWindow()->GetSize().ToVector2f();
			GetRenderer()->GetDefaultCamera2D()->Update();
		};

		input.GetInputFunction("Acelerar") = [this](deltaTime_t dt) {
		};

		ControlsObject = entityComponentSystem->Spawn<OSK::GameObject>();
		ControlsObject->AddComponent<OSK::InputComponent>(input);

		//ENTIDADES

		sound = content->LoadSoundEntity("audio/motorsound.wav");
		
		scene->Load("Levels/startLevel.sc");
			
		arm = scene->Spawn<OSK::GameObject>("arm");
		
		auto cube = scene->GetGameObjectByName<Car>("Cube1");
		{
			cube->GetComponent<OSK::ModelComponent>().AddModel("models/F0/Car/f1.fbx", content);
			auto& model = cube->GetComponent<OSK::ModelComponent>().GetStaticMeshes()[0];
			model.GetTransform()->SetScale(1);
			model.material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
			
			model.material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
			
			model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
			model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
			model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
			
			model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/F0/Car/NullSurface_Color.png"));
			model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/F0/Car/NullSurface_Color.png"));
			
			model.material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);
			
			model.material->FlushUpdate();

			cube->GetComponent<OSK::ModelComponent>().Link(cube->GetTransform());

			auto fr = scene->Spawn<Wheel>();
			auto fl = scene->Spawn<Wheel>();
			auto br = scene->Spawn<Wheel>();
			auto bl = scene->Spawn<Wheel>();

			float scale = 0.01f;
			br->GetComponent<OSK::ModelComponent>().AddModel("models/F0/Wheel/wheel.fbx", content);
			{
				Wheel* w = br;
				auto& model = w->GetComponent<OSK::ModelComponent>().GetStaticMeshes()[0];
				model.GetTransform()->SetScale(scale);
				model.material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
				model.material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);
				model.material->FlushUpdate();

				w->GetComponent<OSK::ModelComponent>().Link(w->GetTransform());
			}
			bl->GetComponent<OSK::ModelComponent>().AddModel("models/F0/Wheel/wheel.fbx", content);
			{
				Wheel* w = bl;
				auto& model = w->GetComponent<OSK::ModelComponent>().GetStaticMeshes()[0];
				model.GetTransform()->SetScale(scale);
				model.material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
				model.material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);
				model.material->FlushUpdate();

				w->GetComponent<OSK::ModelComponent>().Link(w->GetTransform());
			}
			fl->GetComponent<OSK::ModelComponent>().AddModel("models/F0/Wheel/wheel.fbx", content);
			{
				Wheel* w = fl;
				auto& model = w->GetComponent<OSK::ModelComponent>().GetStaticMeshes()[0];
				model.GetTransform()->SetScale(scale);
				model.material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
				model.material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);
				model.material->FlushUpdate();

				w->GetComponent<OSK::ModelComponent>().Link(w->GetTransform());
			}
			fr->GetComponent<OSK::ModelComponent>().AddModel("models/F0/Wheel/wheel.fbx", content);
			{
				Wheel* w = fr;
				auto& model = w->GetComponent<OSK::ModelComponent>().GetStaticMeshes()[0];
				model.GetTransform()->SetScale(scale);
				model.material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
				model.material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
				model.material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/F0/Wheel/Null.1Surface_Color.png"));
				model.material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);
				model.material->FlushUpdate();

				w->GetComponent<OSK::ModelComponent>().Link(w->GetTransform());
			}
		
			fr->GetTransform()->AttachTo(cube->GetTransform());
			fr->GetTransform()->SetPosition({ -0.363, -0.175, 2.14 });
			fr->GetTransform()->RotateWorldSpace(glm::radians(180.0f), {0, 1, 0});

			fl->GetTransform()->AttachTo(cube->GetTransform());
			fl->GetTransform()->SetPosition({ 0.363, -0.175, 2.14 });

			br->GetTransform()->AttachTo(cube->GetTransform());
			br->GetTransform()->SetPosition({ -0.4f, -0.2f, -0.5f });
			br->GetTransform()->RotateWorldSpace(glm::radians(180.0f), { 0, 1, 0 });

			bl->GetTransform()->AttachTo(cube->GetTransform());
			bl->GetTransform()->SetPosition({ 0.4f, -0.2f, -0.5f });

			cube->fr = fr;
			cube->fl = fl;
			cube->br = br;
			cube->bl = bl;
		}

		arm->GetTransform()->SetPosition(cube->GetTransform()->GetPosition() + cube->GetForwardVector() * -0.5f + OSK::Vector3f(0, -1.5f, 0));
		arm->GetTransform()->RotateWorldSpace(glm::radians(-6.0f), { 1, 0, 0 });
		arm->GetTransform()->AttachTo(cube->GetTransform());

		Player = scene->GetGameObjectByName<PlayerCube>("Player");
		Player->GetComponent<OSK::ModelComponent>().AddModel("models/cube/cube.obj", content);
		OSK::ModelComponent& playerModel = Player->GetComponent<OSK::ModelComponent>();
		playerModel.GetStaticMeshes()[0].GetTransform()->SetScale(0.5f);
		playerModel.GetStaticMeshes()[0].GetTransform()->SetPosition(0.5f);
		playerModel.GetStaticMeshes()[0].material = GetRenderer()->GetMaterialSystem()->GetMaterial(OSK::MPIPE_3D)->CreateInstance().GetPointer();
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_CAMERA_3D)->SetBuffer("Camera", GetRenderer()->GetDefaultCamera()->GetUniformBuffer());
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("Lights", renderSystem3D->GetRenderScene()->uboLights);
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetBuffer("DirLightMat", renderSystem3D->GetRenderScene()->shadowMap->uboDirLightMat);
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_SCENE_3D)->SetTexture("ShadowsTexture", renderSystem3D->GetRenderScene()->shadowMap->dirShadows->renderedSprite.GetTexture());
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("models/cube/td.png"));
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("models/cube/ts.png"));
		playerModel.GetStaticMeshes()[0].material->GetMaterialSlot(OSK::MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", renderSystem3D->GetRenderScene()->uboBones);

		playerModel.GetStaticMeshes()[0].material->FlushUpdate();
		playerModel.Link(Player->GetTransform());
		
		scene->GetGameObjectByName("Cube3")->Remove();
		scene->GetGameObjectByName("Cube2")->Remove();

		GetRenderer()->GetDefaultCamera()->GetTransform()->AttachTo(arm->GetTransform());

		physicsSystem->terrainColissionType = OSK::PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY;

		{
			userInterface = new OSK::UiElement();
			userInterface->layout = OSK::UiLayout::FIXED;
			userInterface->shrinkToChildren = false;
			userInterface->SetSize(GetWindow()->GetSize().ToVector2f());

			OSK::UiElement* topbar = new OSK::UiElement();
			topbar->layout = OSK::UiLayout::HORIZONTAL;
			topbar->SetPositionFromAnchor({ 5.0f });
			content->LoadSprite(&topbar->sprite);
			topbar->sprite.SetTexture(buttonTexture);
			topbar->originalSpireColor = OSK::Color(0.2f) * 0.6f;
			topbar->anchor = OSK::UiAnchor::TOP;

			OSK::UiElement* logo = new OSK::UiElement();
			logo->SetSize(48.0f);
			content->LoadSprite(&logo->sprite);
			logo->sprite.SetTexture(GetRenderer()->OskEngineIconSprite.GetTexture());
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
				content->LoadSprite(&optionsMenu->sprite);
				optionsMenu->sprite.SetTexture(buttonTexture);
				optionsMenu->originalSpireColor = OSK::Color(0.2f) * 0.6f;

				OSK::UiElement* header = new OSK::UiElement();
				header->SetSize({ 34.0f });
				header->fuente = Fuente;
				header->text = " Opciones";
				content->LoadSprite(&header->sprite);
				header->sprite.SetTexture(buttonTexture);
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
					content->LoadSprite(&button->sprite);
					button->sprite.SetTexture(buttonTexture);
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
					content->LoadSprite(&button->sprite);
					button->sprite.SetTexture(buttonTexture);
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
				content->LoadSprite(&header->sprite);
				header->sprite.SetTexture(buttonTexture);
				header->originalSpireColor = OSK::Color(0.1f) * 0.9f;
				header->SetSize({ optionsMenu->GetSize().X - optionsMenu->marging.X * 2 - header->marging.X * 2, optionsMenu->GetSize().Y });

				irAlJuego->SetSize(32);

				OSK::UiElement* button = new OSK::UiElement();
				irAlJuego->SetSize({ irAlJuego->GetSize().X * 1.5f, barSize });
				content->LoadSprite(&irAlJuego->sprite);
				irAlJuego->sprite.SetTexture(content->LoadTexture("assets/editor/play.png"));

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
		
		GetAudioSystem()->PlayAudio(*sound, true);
		//cube->SetSound(sound);
	}

	void OnTick(deltaTime_t deltaTime) override {
		float mouseX = newMouseState.GetPosition().X - oldMouseState.GetPosition().X;
		float mouseY = -newMouseState.GetPosition().Y - oldMouseState.GetPosition().Y;
		GetRenderer()->GetDefaultCamera()->Girar(mouseX, mouseY);

		mouseY = 1;

		auto gamepad = GetWindow()->GetGamepadState(0);

		float sensitivity = 260.0f * deltaTime;
		float x = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_X) * sensitivity;
		float y = gamepad.GetAxisState(OSK::GamepadAxis::RIGHT_Y) * sensitivity;
		GetRenderer()->GetDefaultCamera()->Girar(x, -y);

		auto distanceToCamera = scene->GetGameObjectByName("Cube1")->GetForwardVector() * -5.5f;

		userInterface->SetSize(GetWindow()->GetSize().ToVector2f());
		userInterface->Update(newMouseState.GetMouseRectangle(), newMouseState.IsButtonDown(OSK::ButtonCode::BUTTON_LEFT));

		auto car = scene->GetGameObjectByName<Car>("Cube1");
		glm::mat4 matrix = arm->GetTransform()->AsMatrix();
		matrix = glm::translate(matrix, -distanceToCamera.ToGLM());
		matrix = glm::rotate(matrix, mouseX, { 0, 1, 0 });
		matrix = glm::rotate(matrix, mouseY, { 1, 0, 0 });
		matrix = glm::translate(matrix, distanceToCamera.ToGLM());

		arm->GetTransform()->SetMatrix(matrix);

		if (oldKeyboardState.IsKeyUp(OSK::Key::R) && newKeyboardState.IsKeyDown(OSK::Key::R)) {
			((Car*)scene->GetGameObjectByName("Cube1"))->Reload();
		}

		if (oldKeyboardState.IsKeyUp(OSK::Key::Q) && newKeyboardState.IsKeyDown(OSK::Key::Q)) {
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "GPU memory allocator stats: ");
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "		Total allocations: " + std::to_string(GetRenderer()->GetGpuMemoryAllocator()->stats.totalMemoryAllocations));
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "		Total sub allocations: " + std::to_string(GetRenderer()->GetGpuMemoryAllocator()->stats.totalSubAllocations));
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "		Total reserved size: " + std::to_string(GetRenderer()->GetGpuMemoryAllocator()->stats.totalReservedSize));
			OSK::Logger::Log(OSK::LogMessageLevels::INFO, "		Total used size: " + std::to_string(GetRenderer()->GetGpuMemoryAllocator()->stats.totalUsedSize));
		}

	}

	void OnDraw2D() override {
		spriteBatch.Clear();

		spriteBatch.Begin(GetRenderer()->GetDefaultCamera2D());

		spriteBatch.DrawString(Fuente, "OSKengine " + std::string(OSK::ENGINE_VERSION), 1.0f, OSK::Vector2(0), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::BOTTOM_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);
		spriteBatch.DrawString(Fuente, "FPS " + std::to_string((int)GetFPS() - 1), 1.5f, OSK::Vector2(10), OSK::Color(0.3f, 0.7f, 0.9f), OSK::Anchor::TOP_RIGHT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);

		spriteBatch.DrawString(Fuente, std::to_string(scene->GetGameObjectByName("Cube1")->GetComponent<OSK::PhysicsComponent>().velocity.GetLenght() * 3600 / 1000) + "km/h", 1.5f, OSK::Vector2(10), OSK::Color(0.6f, 0.7f, 0.7f), OSK::Anchor::BOTTOM_LEFT, OSK::Vector4(-1.0f), OSK::TextRenderingLimit::MOVE_TEXT);

		userInterface->Draw(&spriteBatch);

		spriteBatch.End();
	}

	OSK::Font* Fuente = nullptr;
	OSK::Font* ShowFont = nullptr;

	OSK::GameObject* ControlsObject;
	OSK::Texture* buttonTexture = nullptr;

	PlayerCube* Player = nullptr;
	OSK::GameObject* arm = nullptr;

	OSK::SoundEmitterComponent* sound = nullptr;

	UniquePtr<OSK::UiElement> userInterface;

};
