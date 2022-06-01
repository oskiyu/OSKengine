module;

// Local includes
#include <OSKengine/MouseModes.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/Window.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/Logger.h>
#include <OSKengine/KeyboardState.h>
#include <OSKengine/ICommandList.h>
#include <OSkengine/IRenderer.h>

#include <OSKengine/GameObject.h>
#include <OSKengine/EntityComponentSystem.h>
#include <OSKengine/CameraComponent3D.h>
#include <OSKengine/Transform3D.h>
#include <OSKengine/MouseState.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/Model3D.h>

#include <OSKengine/InputManager.h>
#include <OSKengine/Game.h>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/Material.h>
#include <OSKengine/Mesh3D.h>
#include <OSKengine/TerrainRenderSystem.h>

#include <OSKengine/SpriteRenderer.h>
#include <OSKengine/CameraComponent2D.h>
#include <OSKengine/Transform2D.h>
#include <OSKengine/Font.h>
#include <OSKengine/FontInstance.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/MaterialSystem.h>
#include <OSKengine/IMaterialSlot.h>

#include <functional>

export module Game;

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

import Skybox;
import Scene;
import PbrModelLoader;
import CarComponent;
import CarControlMessage;
import CarInputListener;
import CarSystem;
import CameraController;
import GameInputListener;

std::string ToString(const Vector3& vector3) {
	return "{ " + std::to_string(vector3.X) + "; " + std::to_string(vector3.Y) + "; " + std::to_string(vector3.Z) + " }";
}

export class Game : public OSK::IGame {

protected:

	void CreateWindow() override {
		Engine::GetWindow()->Create(800, 600, "Demo");
		Engine::GetWindow()->SetMouseReturnMode(MouseReturnMode::ALWAYS_RETURN);
		Engine::GetWindow()->SetMouseMotionMode(MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Demo", { 0, 0, 0 }, *Engine::GetWindow());
	}

	void RegisterAssets() override {

	}

	void RegisterComponents() override {
		Engine::GetEntityComponentSystem()->RegisterComponent<CarComponent>();
		Engine::GetEntityComponentSystem()->RegisterComponent<CameraControlComponent>();

	}

	void RegisterSystems() override {
		Engine::GetEntityComponentSystem()->RegisterSystem<CarSystem>();
		Engine::GetEntityComponentSystem()->RegisterSystem<CameraControlSystem>();

	}

	void OnCreate() override {
		Engine::GetLogger()->InfoLog("Game started");

		// Load
		Scene::Create();
		Scene::LoadSkybox("Resources/Assets/skybox0.json", "Resources/Assets/cube.json", "GLOBAL");
		PbrModelUtils::LoadMaterial("Resources/material.json");

		// Entitites
		// 
		//		Camera
		cameraObject = Engine::GetEntityComponentSystem()->SpawnObject();
		Engine::GetEntityComponentSystem()->AddComponent<CameraComponent3D>(cameraObject, CameraComponent3D());
		Engine::GetEntityComponentSystem()->AddComponent<CameraControlComponent>(cameraObject, {});
		auto& cameraTransform = Engine::GetEntityComponentSystem()->AddComponent<Transform3D>(cameraObject, Transform3D(cameraObject));
		
		Scene::SetCameraObject(cameraObject);

		//		Model
		modelObject = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& model = Engine::GetEntityComponentSystem()->AddComponent<ModelComponent3D>(modelObject, ModelComponent3D());
		Engine::GetEntityComponentSystem()->AddComponent<CarComponent>(modelObject, CarComponent());
		Engine::GetEntityComponentSystem()->AddComponent<CameraControlComponent>(modelObject, {});
		auto& modelTransform = Engine::GetEntityComponentSystem()->AddComponent<Transform3D>(modelObject, Transform3D(modelObject));
		modelTransform.SetScale(0.2f);
		modelTransform.AddPosition({ 0,0.5f, 0 });
		PbrModelUtils::SetupPbrModel(&model, Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/f1_low.json", "GLOBAL"));
		
		cameraTransform.AddPosition({ -0.25f, 1.2f, -4.0f });
		cameraTransform.AttachToObject(modelObject);
		
		// Input

		carInput.SetControlledCar(modelObject);
		Engine::GetInputManager()->AddListener(&carInput);

		cameraInput.SetControlledCamera(cameraObject);
		Engine::GetInputManager()->AddListener(&cameraInput);

		Engine::GetInputManager()->AddListener(&gameInput);

		// Sprite rendering
		spriteRenderer.SetCommandList(Engine::GetRenderer()->GetCommandList());

		cameraObject2D = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& camera2D = Engine::GetEntityComponentSystem()->AddComponent<CameraComponent2D>(cameraObject2D, {});
		Engine::GetEntityComponentSystem()->AddComponent<Transform2D>(cameraObject2D, { cameraObject2D });
		camera2D.LinkToWindow(OSK::Engine::GetWindow());

		font = Engine::GetAssetManager()->Load<Font>("Resources/Assets/font0.json", "GLOBAL");
		font->SetMaterial(Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json"));
		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", camera2D.GetUniformBuffer());
		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->FlushUpdate();
	}

	void OnTick(TDeltaTime deltaTime) override {
		Scene::UpdateCameraBuffer();

		Engine::GetEntityComponentSystem()->GetComponent<CameraComponent2D>(cameraObject2D).UpdateUniformBuffer(
			Engine::GetEntityComponentSystem()->GetComponent<Transform2D>(cameraObject2D)
		);
	}

	void OnPreRender() override {
		Scene::GetSkybox().Render(Engine::GetRenderer()->GetCommandList());
		Engine::GetEntityComponentSystem()->GetSystem<TerrainRenderSystem>()->Render(Engine::GetRenderer()->GetCommandList());
	}

	void OnPostRender() override {
		spriteRenderer.Begin();
		spriteRenderer.DrawString(*font, 30, "OSKengine Alpha 2", { 20.f, 40.f }, 1, 0, OSK::Color::BLUE());
		spriteRenderer.DrawString(*font, 30, "FPS: " + std::to_string(GetFps()), { 20.0f, 80.f }, 1, 0, OSK::Color::WHITE());
		spriteRenderer.DrawString(*font, 30, "v: " + Engine::GetBuild(), {20, 110.f}, 1, 0, OSK::Color::WHITE());
		spriteRenderer.End();
	}

	void OnExit() override {
		Scene::Delete();

		Engine::GetEntityComponentSystem()->DestroyObject(&modelObject);
		Engine::GetEntityComponentSystem()->DestroyObject(&cameraObject);

		Engine::GetLogger()->InfoLog("Game exited");
	}

private:

	GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;
	GameObjectIndex modelObject = EMPTY_GAME_OBJECT;

	SpriteRenderer spriteRenderer;
	GameObjectIndex cameraObject2D = EMPTY_GAME_OBJECT;
	Font* font = nullptr;

	CarInputListener carInput;
	CameraInputListener cameraInput;
	GameInputListener gameInput;

};
