#pragma once

#include "OSKmacros.h"

#ifdef OSK_DEVELOPMENT

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
#include "Transform2D.h"
#include "Model3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"
#include "CameraComponent2D.h"
#include "KeyboardState.h"
#include "Sprite.h"
#include "MouseState.h"
#include "MouseModes.h"
#include "Mesh3D.h"
#include "CubemapTexture.h"
#include "ICommandList.h"

#include "IRenderpass.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "Vertex2D.h"
#include "Sprite.h"
#include "PushConst2D.h"
#include <ext\matrix_clip_space.hpp>
#include "Font.h"
#include "FontLoader.h"
#include "SpriteRenderer.h"
#include "TextureCoordinates.h"
#include "Vertex3D.h"
#include "TerrainComponent.h"
#include "TerrainRenderSystem.h"

#include "UiElement.h"
#include "UiRenderer.h"

OSK::GRAPHICS::Material* skyboxMaterial = nullptr;
OSK::GRAPHICS::Material* material2d = nullptr;
OSK::GRAPHICS::MaterialInstance* skyboxMaterialInstance = nullptr;
OSK::ASSETS::CubemapTexture* cubemap = nullptr;
OSK::ASSETS::Model3D* cubemapModel = nullptr;

OSK::GRAPHICS::Material* terrainMaterialFill = nullptr;
OSK::GRAPHICS::Material* terrainMaterialLine = nullptr;
OSK::GRAPHICS::Material* terrainMaterial = nullptr;

OSK::GRAPHICS::SpriteRenderer spriteRenderer;

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
		skyboxMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
		material2d = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json");

		texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		uniformBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		
		OSK::ASSETS::Model3D* model = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		OSK::ASSETS::Model3D* model_low = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1_low.json", "GLOBAL");

		// ECS
		ballObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::ECS::Transform3D& transform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(ballObject, OSK::ECS::Transform3D(ballObject));
		OSK::ECS::ModelComponent3D* modelComponent = &OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(ballObject, {});

		transform.AddPosition({ 0, 0.1f, 0 });
		transform.SetScale(0.03f);

		modelComponent->SetModel(model);
		modelComponent->SetMaterial(material);

		modelComponent->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent->BindTextureForAllMeshes("global", "stexture", texture);

		for (TSize i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& metadata = model->GetMetadata().meshesMetadata[i];

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImage("stexture", model->GetImage(texture.second));

				modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();
			}
		}

		cameraObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::CameraComponent3D>(cameraObject, {});
		auto& cameraTransform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(cameraObject, OSK::ECS::Transform3D(cameraObject));
		//cameraTransform.AttachToObject(ballObject);

		// ECS 2
		smallBallObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		auto& transform2 = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(smallBallObject, OSK::ECS::Transform3D(smallBallObject));
		auto modelComponent2 = &OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(smallBallObject, {});

		transform2.AddPosition({ 0, 1, 1.4f });
		transform2.SetScale(0.05f);

		modelComponent2->SetModel(model_low);
		modelComponent2->SetMaterial(material);

		modelComponent2->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent2->BindTextureForAllMeshes("global", "stexture", texture);

		// Cubemap
		cubemap = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::CubemapTexture>("Resources/Assets/skybox0.json", "GLOBAL");
		cubemapModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/cube.json", "GLOBAL");

		skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();
		skyboxMaterialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		skyboxMaterialInstance->GetSlot("global")->SetGpuImage("skybox", cubemap->GetGpuImage());
		skyboxMaterialInstance->GetSlot("global")->FlushUpdate();

		cameraObject2d = OSK::Engine::GetEntityComponentSystem()->SpawnObject();
		auto& camera2D = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::CameraComponent2D>(cameraObject2d, {});
		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform2D>(cameraObject2d, { cameraObject2d });
		camera2D.LinkToWindow(OSK::Engine::GetWindow());

		spriteObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();
		auto& comp = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::GRAPHICS::Sprite>(spriteObject, {});
		auto& tr2d = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform2D>(spriteObject, { cameraObject2d });
		comp.SetTexCoords(OSK::GRAPHICS::TextureCoordinates2D::Normalized({ 0.5f, 0.5f, 1.0f, 1.0f }));
		tr2d.SetScale({ 100, 120 });
		comp.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		comp.SetCamera(camera2D);
		comp.SetTexture(texture);

		spriteRenderer.SetCommandList(OSK::Engine::GetRenderer()->GetCommandList());

		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", camera2D.GetUniformBuffer());
		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		// Terrain
		terrain = OSK::Engine::GetEntityComponentSystem()->SpawnObject();
		auto& terrainComponent = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::TerrainComponent>(terrain, {});
		auto& terrainTransform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(terrain, { terrain });
		terrainComponent.Generate({ 100 });

		terrainMaterialFill = terrainMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_terrain.json");
		terrainMaterialLine = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_terrain_lines.json");

		terrainComponent.SetMaterialInstance(terrainMaterial->CreateInstance());

		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("heightmap",
			OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/heightmap0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("texture",
			OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/terrain0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		terrainTransform.SetScale({ 10, 1, 10 });

		// UI
		mainUi = new OSK::UI::UiElement;
		mainUi->sprite.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		mainUi->sprite.SetCamera(camera2D);
		mainUi->sprite.SetTexture(texture);
		mainUi->SetPosition({ 100, 80 });
		mainUi->SetSize({ 40, 40 });
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

		// Car
		auto& carTransform = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject);
		if (newKs->IsKeyDown(OSK::IO::Key::UP))
			carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * 2);
		if (newKs->IsKeyDown(OSK::IO::Key::DOWN))
			carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * -2);
		if (newKs->IsKeyDown(OSK::IO::Key::LEFT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
		if (newKs->IsKeyDown(OSK::IO::Key::RIGHT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, -1, 0 });

		if (newKs->IsKeyDown(OSK::IO::Key::F11) && oldKs->IsKeyUp(OSK::IO::Key::F11))
			OSK::Engine::GetWindow()->ToggleFullScreen();

		if (newKs->IsKeyDown(OSK::IO::Key::F1) && oldKs->IsKeyUp(OSK::IO::Key::F1)) {
			if (terrainMaterial == terrainMaterialFill)
				terrainMaterial = terrainMaterialLine;
			else
				terrainMaterial = terrainMaterialFill;
		}

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
		uniformBuffer->Write(cameraTransform.GetPosition());
		uniformBuffer->Unmap();

		OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform2D>(cameraObject2d)
		);

	}

	void OnPreRender() override {
		auto commandList = OSK::Engine::GetRenderer()->GetCommandList();

		commandList->BindMaterial(skyboxMaterial);
		commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("global"));
		commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		commandList->DrawSingleInstance(cubemapModel->GetIndexCount());

		OSK::Engine::GetEntityComponentSystem()->GetSystem<OSK::ECS::TerrainRenderSystem>()->Render(commandList);
	}

	void OnPostRender() override {
		auto commandList = OSK::Engine::GetRenderer()->GetCommandList();

		spriteRenderer.Begin();
		spriteRenderer.DrawString(*font, 30, "FPS: " + std::to_string(GetFps()), { 300.0f, 50.f }, 1, 0, OSK::Color::BLUE());
		spriteRenderer.DrawString(*font, 30, "MS: " + std::to_string(1000.0f / GetFps()), { 300.0f, 80.f }, 1, 0, OSK::Color::BLUE());
		spriteRenderer.End();

		uiRenderer.Render(commandList, mainUi.GetPointer());
	}

	void OnExit() override {
		uniformBuffer.Delete();

		delete skyboxMaterialInstance;
	}

private:

	OSK::ECS::GameObjectIndex ballObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex smallBallObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex spriteObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject2d = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex terrain = OSK::ECS::EMPTY_GAME_OBJECT;

	OSK::UniquePtr<OSK::GRAPHICS::IGpuUniformBuffer> uniformBuffer;
	
	OSK::ASSETS::Texture* texture = nullptr;
	OSK::ASSETS::Font* font = nullptr;

	OSK::UI::UiRenderer uiRenderer;
	OSK::UniquePtr<OSK::UI::UiElement> mainUi;

};

#endif 