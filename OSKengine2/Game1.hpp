#pragma once

#include "Game.h"
#include "OSKengine.h"
#include "Window.h"
#include "IRenderer.h"
#include "GameObject.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "OSKengine.h"
#include "Texture.h"
#include "AssetManager.h"
#include "IMaterialSlot.h"
#include "IGpuUniformBuffer.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "IGpuMemoryAllocator.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "Model3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"
#include "KeyboardState.h"
#include "MouseState.h"
#include "MouseModes.h"

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		OSK::Engine::GetWindow()->Create(800, 600, "OSKengine");
		OSK::Engine::GetWindow()->SetMouseReturnMode(OSK::IO::MouseReturnMode::ALWAYS_RETURN);
		OSK::Engine::GetWindow()->SetMouseMotionMode(OSK::IO::MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		OSK::Engine::GetRenderer()->Initialize("Game", {}, *OSK::Engine::GetWindow());
	}

	void OnCreate() override {
		// Material load
		OSK::GRAPHICS::Material* material = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material.json");
		OSK::ASSETS::Texture* texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		uniformBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();

		auto materialInstance = material->CreateInstance();
		materialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		materialInstance->GetSlot("global")->SetTexture("texture", texture);
		materialInstance->GetSlot("global")->FlushUpdate();

		OSK::ASSETS::Model3D* model = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/model0.json", "GLOBAL");

		// ECS
		ballObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::ECS::Transform3D& transform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(ballObject, OSK::ECS::Transform3D(ballObject));
		OSK::ECS::ModelComponent3D& modelComponent = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(ballObject, {});

		transform.AddPosition({ 0, 1, 1 });
		transform.SetScale(0.8f);

		modelComponent.SetModel(model);
		modelComponent.SetMaterialInstance(materialInstance);

		cameraObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::CameraComponent3D>(cameraObject, {});
		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(cameraObject, OSK::ECS::Transform3D(cameraObject));

		// ECS 2
		smallBallObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		auto& transform2 = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(smallBallObject, OSK::ECS::Transform3D(smallBallObject));
		auto& modelComponent2 = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(smallBallObject, {});

		transform2.AddPosition({ 0, 1, 3 });
		transform2.SetScale(0.5f);

		auto materialInstance2 = material->CreateInstance();
		materialInstance2->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		materialInstance2->GetSlot("global")->SetTexture("texture", texture);
		materialInstance2->GetSlot("global")->FlushUpdate();

		modelComponent2.SetModel(model);
		modelComponent2.SetMaterialInstance(materialInstance2);
	}

	void OnTick(TDeltaTime deltaTime) override {
		OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject).RotateLocalSpace(deltaTime, { 0 ,1 ,0 });

		OSK::ECS::CameraComponent3D& camera = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::CameraComponent3D>(cameraObject);
		OSK::ECS::Transform3D& cameraTransform = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(cameraObject);

		float forwardMovement = 0.0f;
		float rightMovement = 0.0f;
		auto newKs = OSK::Engine::GetWindow()->GetKeyboardState();
		auto oldKs = OSK::Engine::GetWindow()->GetPreviousKeyboardState();
		if (newKs->IsKeyDown(OSK::IO::Key::W))
			forwardMovement += 1.0f;
		if (newKs->IsKeyDown(OSK::IO::Key::S))
			forwardMovement -= 1.0f;
		if (newKs->IsKeyDown(OSK::IO::Key::A))
			rightMovement -= 1.0f;
		if (newKs->IsKeyDown(OSK::IO::Key::D))
			rightMovement += 1.0f;

		if (newKs->IsKeyDown(OSK::IO::Key::F11) && oldKs->IsKeyUp(OSK::IO::Key::F11))
			OSK::Engine::GetWindow()->ToggleFullScreen();

		if (newKs->IsKeyDown(OSK::IO::Key::ESCAPE))
			this->Exit();

		camera.Rotate(
			(OSK::Engine::GetWindow()->GetMouseState()->GetPosition().X - OSK::Engine::GetWindow()->GetPreviousMouseState()->GetPosition().X),
			(OSK::Engine::GetWindow()->GetMouseState()->GetPosition().Y - OSK::Engine::GetWindow()->GetPreviousMouseState()->GetPosition().Y)
		);

		cameraTransform.AddPosition(cameraTransform.GetForwardVector().GetNormalized() * forwardMovement * deltaTime);
		cameraTransform.AddPosition(cameraTransform.GetRightVector().GetNormalized() * rightMovement * deltaTime);
		camera.UpdateTransform(&cameraTransform);

		uniformBuffer->ResetCursor();
		uniformBuffer->MapMemory();
		uniformBuffer->Write(camera.GetProjectionMatrix(cameraTransform));
		uniformBuffer->Write(camera.GetViewMatrix());
		uniformBuffer->Unmap();
	}

	void OnExit() override {
		uniformBuffer.Delete();
	}

private:

	OSK::ECS::GameObjectIndex ballObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex smallBallObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;

	OSK::UniquePtr<OSK::GRAPHICS::IGpuUniformBuffer> uniformBuffer;

};
