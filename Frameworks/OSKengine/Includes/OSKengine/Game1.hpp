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
#include "TopLevelAccelerationStructureVulkan.h"
#include "IGpuMemorySubblock.h"
#include "UiElement.h"
#include "UiRenderer.h"
#include "Viewport.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "RenderTarget.h"

OSK::GRAPHICS::Material* rtMaterial = nullptr;
OSK::GRAPHICS::MaterialInstance* rtMaterialInstance = nullptr;
OSK::GRAPHICS::GpuImage* rtTargetImage = nullptr;

OSK::GRAPHICS::Material* material = nullptr;
OSK::GRAPHICS::Material* skyboxMaterial = nullptr;
OSK::GRAPHICS::Material* material2d = nullptr;
OSK::GRAPHICS::MaterialInstance* skyboxMaterialInstance = nullptr;
OSK::ASSETS::CubemapTexture* cubemap = nullptr;
OSK::ASSETS::Model3D* cubemapModel = nullptr;

OSK::GRAPHICS::Material* terrainMaterialFill = nullptr;
OSK::GRAPHICS::Material* terrainMaterialLine = nullptr;
OSK::GRAPHICS::Material* terrainMaterial = nullptr;

OSK::GRAPHICS::SpriteRenderer spriteRenderer;

OSK::GRAPHICS::ITopLevelAccelerationStructure* topLevelAccelerationStructure = nullptr;
OSK::GRAPHICS::GpuDataBuffer* instancesInfoBuffer = nullptr;

OSK::GRAPHICS::IRenderpass* normalRenderpass = nullptr;
OSK::GRAPHICS::GpuImage* normalImage = nullptr;

OSK::GRAPHICS::RenderTarget skyboxRenderTarget;

struct RtInstanceInfo {
	TSize vertexOffset = 0; // In bytes
	TSize indexOffset = 0; // In bytes
};

// PBR
using namespace OSK;
using namespace OSK::GRAPHICS;
Material* pbrColorMaterial = nullptr;
Material* pbrNormalMaterial = nullptr;

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		OSK::Engine::GetWindow()->Create(800, 600, "OSKengine");
		OSK::Engine::GetWindow()->SetMouseReturnMode(OSK::IO::MouseReturnMode::ALWAYS_RETURN);
		OSK::Engine::GetWindow()->SetMouseMotionMode(OSK::IO::MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		OSK::Engine::GetRenderer()->Initialize("Game", {}, *OSK::Engine::GetWindow(), OSK::GRAPHICS::PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
		// Material load
		pbrColorMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/color.json");
		pbrNormalMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/normal.json");

		material = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material.json");
		skyboxMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
		material2d = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json");

		normalImage = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
			OSK::GRAPHICS::GpuImageUsage::COLOR | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
		normalRenderpass = OSK::Engine::GetRenderer()->CreateSecondaryRenderpass(normalImage).GetPointer();
		normalRenderpass->SetImages(normalImage, normalImage, normalImage);

		texture = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		uniformBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		
		OSK::ASSETS::Model3D* model = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		OSK::ASSETS::Model3D* model_low = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>("Resources/Assets/f1_low.json", "GLOBAL");

		topLevelAccelerationStructure = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateTopAccelerationStructure({
			model->GetAccelerationStructure(),
			model_low->GetAccelerationStructure()
			}).GetPointer();

		/**/OSK::DynamicArray<RtInstanceInfo> instancesInfo;
		instancesInfo.Insert({
			model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfo.Insert({ 
			model_low->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model_low->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfoBuffer = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateBuffer(sizeof(RtInstanceInfo) * 4, 0, OSK::GRAPHICS::GpuBufferUsage::STORAGE_BUFFER, OSK::GRAPHICS::GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
		instancesInfoBuffer->MapMemory();
		instancesInfoBuffer->Write(instancesInfo.GetData(), instancesInfo.GetSize() * sizeof(RtInstanceInfo));
		instancesInfoBuffer->Unmap();/**/

		rtTargetImage = OSK::Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, OSK::GRAPHICS::GpuImageDimension::d2D, 1, OSK::GRAPHICS::Format::RGBA8_UNORM,
			OSK::GRAPHICS::GpuImageUsage::RT_TARGET_IMAGE | OSK::GRAPHICS::GpuImageUsage::SAMPLED, OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
		rtMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rt.json");
		rtMaterialInstance = rtMaterial->CreateInstance().GetPointer();
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("vertices", model->GetVertexBuffer());
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("indices", model->GetIndexBuffer());
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("instanceInfos", instancesInfoBuffer);
		rtMaterialInstance->GetSlot("rt")->SetAccelerationStructure("topLevelAccelerationStructure", topLevelAccelerationStructure);
		rtMaterialInstance->GetSlot("rt")->SetStorageImage("targetImage", rtTargetImage);
		rtMaterialInstance->GetSlot("rt")->FlushUpdate();
		rtMaterialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		rtMaterialInstance->GetSlot("global")->FlushUpdate();


		// ECS
		ballObject = OSK::Engine::GetEntityComponentSystem()->SpawnObject();

		OSK::ECS::Transform3D& transform = OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::Transform3D>(ballObject, OSK::ECS::Transform3D(ballObject));
		OSK::ECS::ModelComponent3D* modelComponent = &OSK::Engine::GetEntityComponentSystem()->AddComponent<OSK::ECS::ModelComponent3D>(ballObject, {});

		transform.AddPosition({ 0, 0.1f, 0 });
		//transform.SetScale(0.03f);

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
		comp.SetTexCoords(OSK::GRAPHICS::TextureCoordinates2D::Normalized({ 0.0f, 0.0f, 1.0f, 1.0f }));
		tr2d.SetScale({ 800 / 2, 600 / 2 });
		comp.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		comp.SetCamera(camera2D);
		//comp.SetTexture(texture);
		comp.SetGpuImage(normalImage);

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
		/*mainUi->sprite.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		mainUi->sprite.SetCamera(camera2D);
		mainUi->sprite.SetGpuImage(rtTargetImage);
		mainUi->SetPosition({ 100, 80 });
		mainUi->SetSize({ 40, 40 });*/

		skyboxRenderTarget.Create(Engine::GetWindow()->GetWindowSize());
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
		if (newKs->IsKeyDown(OSK::IO::Key::P))
			this->Exit();

		camera.Rotate(
			(float)(OSK::Engine::GetWindow()->GetMouseState()->GetPosition().X - OSK::Engine::GetWindow()->GetPreviousMouseState()->GetPosition().X),
			(float)(OSK::Engine::GetWindow()->GetMouseState()->GetPosition().Y - OSK::Engine::GetWindow()->GetPreviousMouseState()->GetPosition().Y)
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

		const auto& transformComponent = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(ballObject);
		auto& modelComponent = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::ModelComponent3D>(ballObject);
		modelComponent.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent.GetAsMatrix());
		modelComponent.GetModel()->GetAccelerationStructure()->Update();

		const auto& transformComponent2 = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::Transform3D>(smallBallObject);
		auto& modelComponent2 = OSK::Engine::GetEntityComponentSystem()->GetComponent<OSK::ECS::ModelComponent3D>(smallBallObject);
		modelComponent2.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent2.GetAsMatrix());
		modelComponent2.GetModel()->GetAccelerationStructure()->Update();
		topLevelAccelerationStructure->Update();

		auto commandList = OSK::Engine::GetRenderer()->GetCommandList();
		commandList->TransitionImageLayout(rtTargetImage, OSK::GRAPHICS::GpuImageLayout::GENERAL, 0, 1);
		commandList->BindMaterial(rtMaterial);
		for (auto const& slotName : rtMaterialInstance->GetLayout()->GetAllSlotNames())
			commandList->BindMaterialSlot(rtMaterialInstance->GetSlot(slotName));
		commandList->TraceRays(0, 0, 0, { 1920, 1080 });
		commandList->TransitionImageLayout(rtTargetImage, OSK::GRAPHICS::GpuImageLayout::SHADER_READ_ONLY, 0, 1);
	}

	void BuildFrame() override {
		auto commandList = Engine::GetRenderer()->GetCommandList();
		auto renderpass = Engine::GetRenderer()->GetFinalRenderpass();

		static SpriteRenderer spriteRenderer{};
		spriteRenderer.SetCommandList(commandList);

		Vector4ui windowRec = {
			0,
			0,
			Engine::GetWindow()->GetWindowSize().X,
			Engine::GetWindow()->GetWindowSize().Y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		commandList->SetViewport(viewport);
		commandList->SetScissor(windowRec);

		commandList->BeginRenderpass(renderpass);

		// Render skybox
		commandList->BindMaterial(skyboxMaterial);
		commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("global"));
		commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		commandList->DrawSingleInstance(cubemapModel->GetIndexCount());

		spriteRenderer.Begin();

		commandList->BindMaterial(material2d);

		ECS::Transform2D spriteTransform{ ECS::EMPTY_GAME_OBJECT };
		spriteTransform.SetScale(Engine::GetWindow()->GetWindowSize().ToVector2f());

		spriteRenderer.Draw(Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().GetSprite(), spriteTransform);
		spriteRenderer.Draw(Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem2D>()->GetRenderTarget().GetSprite(), spriteTransform);

		spriteRenderer.DrawString(*font, 30, "OSKengine build " + Engine::GetBuild(), Vector2f{ 20.0f, 30.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());
		spriteRenderer.DrawString(*font, 30, "FPS " + std::to_string(GetFps()), Vector2f{20.0f, 60.0f}, Vector2f{1.0f}, 0.0f, Color::WHITE());

		spriteRenderer.End();

		commandList->EndRenderpass(renderpass);
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
