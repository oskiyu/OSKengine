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

OSK::GRAPHICS::Material* skyboxMaterial = nullptr;
OSK::GRAPHICS::MaterialInstance* skyboxMaterialInstance = nullptr;
OSK::ASSETS::CubemapTexture* cubemap = nullptr;
OSK::ASSETS::Model3D* cubemapModel = nullptr;

OSK::GRAPHICS::GpuImage* renderpassImage = nullptr;
OSK::GRAPHICS::IRenderpass* renderpass = nullptr;

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		OSK::Engine::GetWindow()->Create(800, 600, "OSKengine");
		OSK::Engine::GetWindow()->SetMouseReturnMode(OSK::IO::MouseReturnMode::ALWAYS_RETURN);
		OSK::Engine::GetWindow()->SetMouseMotionMode(OSK::IO::MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		OSK::Engine::GetRenderer()->Initialize("Game", {}, *OSK::Engine::GetWindow());

		OSK::DynamicArray<OSK::GRAPHICS::Vertex2D> vertices2d = {
			{ { 0, 0 }, { 0, 0 } },
			{ { 0, 1 }, { 0, 1 } },
			{ { 1, 0 }, { 1, 0 } },
			{ { 1, 1 }, { 1, 1 } }
		};

		OSK::DynamicArray<OSK::GRAPHICS::TIndexSize> indices2d = {
			0, 1, 2, 1, 2, 3
		};

		OSK::GRAPHICS::Vertex2D::globalVertexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices2d).GetPointer();
		OSK::GRAPHICS::Vertex2D::globalIndexBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices2d).GetPointer();
	}

	void OnCreate() override {
		// Material load
		OSK::GRAPHICS::Material* material = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material.json");
		OSK::ASSETS::Texture* texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		uniformBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();

		OSK::ASSETS::Model3D* model = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		OSK::ASSETS::Model3D* model_low = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1_low.json", "GLOBAL");

		// ECS
		ballObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::ECS::Transform3D& transform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(ballObject, OSK::ECS::Transform3D(ballObject));
		OSK::ECS::ModelComponent3D* modelComponent = &OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(ballObject, {});

		transform.AddPosition({ 0, 1, 1 });
		transform.SetScale(0.05f);

		modelComponent->SetModel(model);
		modelComponent->SetMaterial(material);

		modelComponent->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent->BindTextureForAllMeshes("global", "texture", texture);

		for (TSize i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& metadata = model->GetMetadata().meshesMetadata[i];

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImage("texture", model->GetImage(texture.second));

				modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();
			}
		}

		cameraObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::CameraComponent3D>(cameraObject, {});
		OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(cameraObject, OSK::ECS::Transform3D(cameraObject));

		// ECS 2
		smallBallObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		auto& transform2 = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(smallBallObject, OSK::ECS::Transform3D(smallBallObject));
		auto modelComponent2 = &OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(smallBallObject, {});

		transform2.AddPosition({ 0, 1, 1.4f });
		transform2.SetScale(0.05f);

		modelComponent2->SetModel(model_low);
		modelComponent2->SetMaterial(material);

		modelComponent2->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent2->BindTextureForAllMeshes("global", "texture", texture);

		// Cubemap
		cubemap = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::CubemapTexture>("Resources/Assets/skybox0.json", "GLOBAL");
		cubemapModel = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/cube.json", "GLOBAL");
		skyboxMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");

		skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();
		skyboxMaterialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		skyboxMaterialInstance->GetSlot("global")->SetGpuImage("cubemap", cubemap->GetGpuImage());
		skyboxMaterialInstance->GetSlot("global")->FlushUpdate();

		renderpassImage = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D,
			1, OSK::GRAPHICS::Format::RGBA8_UNORM, OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
		renderpass = OSK::Engine::GetRenderer()->CreateSecondaryRenderpass(renderpassImage).GetPointer();
	}

	void OnTick(TDeltaTime deltaTime) override {
		//OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject).RotateLocalSpace(deltaTime, { 0 ,1 ,0 });

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
		uniformBuffer->Write(cameraTransform.GetPosition());
		uniformBuffer->Unmap();
	}

	void OnRender() override {
		auto commandList = OSK::Engine::GetRenderer()->GetCommandList();

		commandList->BindMaterial(skyboxMaterial);
		commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("global"));
		commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		commandList->DrawSingleInstance(cubemapModel->GetIndexCount());
	}

	void OnExit() override {
		uniformBuffer.Delete();

		delete skyboxMaterialInstance;
		delete renderpass;
		delete renderpassImage;
	}

private:

	OSK::ECS::GameObjectIndex ballObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex smallBallObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;

	OSK::UniquePtr<OSK::GRAPHICS::IGpuUniformBuffer> uniformBuffer;

};
