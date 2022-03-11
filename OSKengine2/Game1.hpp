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

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		OSK::Engine::GetWindow()->Create(800, 600, "OSKengine");
	}

	void SetupEngine() override {
		OSK::Engine::GetRenderer()->Initialize("Game", {}, *OSK::Engine::GetWindow());
	}

	void OnCreate() override {
		// Material load
		OSK::GRAPHICS::Material* material = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material.json");
		OSK::ASSETS::Texture* texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		uniformBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4)).GetPointer();

		OSK::OwnedPtr<OSK::GRAPHICS::MaterialInstance> materialInstance = material->CreateInstance();
		materialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		materialInstance->GetSlot("global")->SetTexture("texture", texture);
		materialInstance->GetSlot("global")->FlushUpdate();

		OSK::ASSETS::Model3D* model = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/model0.json", "GLOBAL");

		// ECS
		ballObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();
		
		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(ballObject, OSK::ECS::Transform3D(ballObject));
		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(ballObject, {});

		OSK::ECS::Transform3D& transform = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject);
		transform.AddPosition({ 0, 1, 1 });
		transform.SetScale(0.8f);

		OSK::ECS::ModelComponent3D& modelComponent = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::ModelComponent3D>(ballObject);
		modelComponent.SetModel(model);
		modelComponent.SetMaterialInstance(materialInstance);

	}

	void OnTick(TDeltaTime deltaTime) override {
		OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject).RotateLocalSpace(deltaTime, { 0 ,1 ,0 });
	}

private:

	OSK::ECS::GameObjectIndex ballObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::UniquePtr<OSK::GRAPHICS::IGpuUniformBuffer> uniformBuffer;

};
