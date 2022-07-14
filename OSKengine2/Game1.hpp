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
#include "Lights.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "RenderTarget.h"

#include "IrradianceMap.h"
#include "IrradianceMapLoader.h"

OSK::GRAPHICS::Material* rtMaterial = nullptr;
OSK::GRAPHICS::MaterialInstance* rtMaterialInstance = nullptr;
OSK::GRAPHICS::GpuImage* rtTargetImage = nullptr;

OSK::GRAPHICS::Material* material = nullptr;
OSK::GRAPHICS::Material* skyboxMaterial = nullptr;
OSK::GRAPHICS::Material* material2d = nullptr;
OSK::GRAPHICS::Material* materialRenderTarget = nullptr;
OSK::GRAPHICS::MaterialInstance* skyboxMaterialInstance = nullptr;
OSK::ASSETS::CubemapTexture* cubemap = nullptr;
OSK::ASSETS::Model3D* cubemapModel = nullptr;

OSK::GRAPHICS::Material* terrainMaterialFill = nullptr;
OSK::GRAPHICS::Material* terrainMaterialLine = nullptr;
OSK::GRAPHICS::Material* terrainMaterial = nullptr;

OSK::GRAPHICS::SpriteRenderer spriteRenderer;

OSK::GRAPHICS::ITopLevelAccelerationStructure* topLevelAccelerationStructure = nullptr;
OSK::GRAPHICS::GpuDataBuffer* instancesInfoBuffer = nullptr;

OSK::GRAPHICS::GpuImage* normalImage = nullptr;

OSK::GRAPHICS::RenderTarget skyboxRenderTarget;
OSK::GRAPHICS::RenderTarget textRenderTarget;

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
		Engine::GetWindow()->Create(800, 600, "OSKengine");
		Engine::GetWindow()->SetMouseReturnMode(IO::MouseReturnMode::ALWAYS_RETURN);
		Engine::GetWindow()->SetMouseMotionMode(IO::MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetWindow(), PresentMode::VSYNC_ON_TRIPLE_BUFFER);
	}

	void OnCreate() override {
		// Material load

		Engine::GetLogger()->DebugLog("START");
		auto irradianceMap = Engine::GetAssetManager()->Load<ASSETS::IrradianceMap>("Resources/Assets/IBL/irradiance0.json", "GLOBAL");
		Engine::GetLogger()->DebugLog("END");

		Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_irradiance_gen.json");
		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_pbr.json");
		skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
		material2d = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json");
		materialRenderTarget = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rendertarget.json");

		texture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		uniformBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		
		ASSETS::Model3D* model = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		ASSETS::Model3D* model_low = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/f1_low.json", "GLOBAL");

		topLevelAccelerationStructure = Engine::GetRenderer()->GetMemoryAllocator()->CreateTopAccelerationStructure({
			model->GetAccelerationStructure(),
			model_low->GetAccelerationStructure()
			}).GetPointer();

		DynamicArray<RtInstanceInfo> instancesInfo;
		instancesInfo.Insert({
			model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfo.Insert({ 
			model_low->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model_low->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfoBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateBuffer(sizeof(RtInstanceInfo) * 4, 0, OSK::GRAPHICS::GpuBufferUsage::STORAGE_BUFFER, OSK::GRAPHICS::GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
		instancesInfoBuffer->MapMemory();
		instancesInfoBuffer->Write(instancesInfo.GetData(), instancesInfo.GetSize() * sizeof(RtInstanceInfo));
		instancesInfoBuffer->Unmap();/**/

		rtTargetImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, GpuImageDimension::d2D, 1, Format::RGBA8_UNORM,
				GpuImageUsage::RT_TARGET_IMAGE | GpuImageUsage::SAMPLED, GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
		rtMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rt.json");
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
		ballObject = Engine::GetEntityComponentSystem()->SpawnObject();

		ECS::Transform3D& transform = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(ballObject, ECS::Transform3D(ballObject));
		ECS::ModelComponent3D* modelComponent = &Engine::GetEntityComponentSystem()->AddComponent<ECS::ModelComponent3D>(ballObject, {});

		transform.AddPosition({ 0, 0.1f, 0 });
		//transform.SetScale(0.03f);

		DirectionalLight dirLight;
		dirLight.direction = Vector3f(1.0f, -3.f, 0.0f).GetNormalized();
		dirLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);
		dirLight.intensity = 1.8f;

		dirLightBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
		dirLightBuffer->MapMemory();
		dirLightBuffer->Write(dirLight);
		dirLightBuffer->Unmap();

		modelComponent->SetModel(model);
		modelComponent->SetMaterial(material);

		modelComponent->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent->BindTextureForAllMeshes("global", "stexture", texture);
		modelComponent->BindGpuImageForAllMeshes("global", "irradianceMap", irradianceMap->GetGpuImage());
		modelComponent->BindUniformBufferForAllMeshes("global", "dirLight", dirLightBuffer.GetPointer());

		for (TSize i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& metadata = model->GetMetadata().meshesMetadata[i];

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImage("stexture", model->GetImage(texture.second));

				modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();
			}
		}

		cameraObject = Engine::GetEntityComponentSystem()->SpawnObject();

		Engine::GetEntityComponentSystem()->AddComponent<ECS::CameraComponent3D>(cameraObject, {});
		auto& cameraTransform = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(cameraObject, ECS::Transform3D(cameraObject));
		//cameraTransform.AttachToObject(ballObject);

		// ECS 2
		smallBallObject = Engine::GetEntityComponentSystem()->SpawnObject();

		auto& transform2 = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(smallBallObject, ECS::Transform3D(smallBallObject));
		auto modelComponent2 = &Engine::GetEntityComponentSystem()->AddComponent<ECS::ModelComponent3D>(smallBallObject, {});

		transform2.AddPosition({ 0, 1, 1.4f });
		transform2.SetScale(0.05f);

		modelComponent2->SetModel(model_low);
		modelComponent2->SetMaterial(material);

		modelComponent2->BindUniformBufferForAllMeshes("global", "camera", uniformBuffer.GetPointer());
		modelComponent2->BindTextureForAllMeshes("global", "stexture", texture);
		modelComponent2->BindGpuImageForAllMeshes("global", "irradianceMap", irradianceMap->GetGpuImage());
		modelComponent2->BindUniformBufferForAllMeshes("global", "dirLight", dirLightBuffer.GetPointer());

		// Cubemap
		cubemap = Engine::GetAssetManager()->Load<ASSETS::CubemapTexture>("Resources/Assets/skybox0.json", "GLOBAL");
		cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/cube.json", "GLOBAL");

		skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();
		skyboxMaterialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		skyboxMaterialInstance->GetSlot("global")->SetGpuImage("skybox", cubemap->GetGpuImage());
		skyboxMaterialInstance->GetSlot("global")->FlushUpdate();

		cameraObject2d = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& camera2D = Engine::GetEntityComponentSystem()->AddComponent<ECS::CameraComponent2D>(cameraObject2d, {});
		Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform2D>(cameraObject2d, { cameraObject2d });
		camera2D.LinkToWindow(Engine::GetWindow());

		spriteObject = Engine::GetEntityComponentSystem()->SpawnObject();


		spriteRenderer.SetCommandList(Engine::GetRenderer()->GetCommandList());

		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", camera2D.GetUniformBuffer());
		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		// Terrain
		terrain = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& terrainComponent = Engine::GetEntityComponentSystem()->AddComponent<ECS::TerrainComponent>(terrain, {});
		auto& terrainTransform = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(terrain, { terrain });
		terrainComponent.Generate({ 100 });

		terrainMaterialFill = terrainMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_terrain.json");
		terrainMaterialLine = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_terrain_lines.json");

		terrainComponent.SetMaterialInstance(terrainMaterial->CreateInstance());

		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer.GetPointer());
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("heightmap",
			Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/heightmap0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetTexture("texture",
			Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/terrain0.json", "GLOBAL"));
		terrainComponent.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		terrainTransform.SetScale({ 10, 1, 10 });

		// UI
		mainUi = new UI::UiElement;
		/*mainUi->sprite.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		mainUi->sprite.SetCamera(camera2D);
		mainUi->sprite.SetGpuImage(rtTargetImage);
		mainUi->SetPosition({ 100, 80 });
		mainUi->SetSize({ 40, 40 });*/

		skyboxRenderTarget.Create(Engine::GetWindow()->GetWindowSize(), Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);
		textRenderTarget.Create(Engine::GetWindow()->GetWindowSize(), Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);

		Engine::GetRenderer()->RegisterRenderTarget(&skyboxRenderTarget);
		Engine::GetRenderer()->RegisterRenderTarget(&textRenderTarget);

		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().SetResolutionScale(1.0f);
	}

	void RegisterSystems() override {
		Engine::GetEntityComponentSystem()->RemoveSystem<ECS::RenderSystem2D>();
	}

	void OnTick(TDeltaTime deltaTime) override {
		ECS::CameraComponent3D& camera = Engine::GetEntityComponentSystem()->GetComponent<ECS::CameraComponent3D>(cameraObject);
		ECS::Transform3D& cameraTransform = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(cameraObject);

		float forwardMovement = 0.0f;
		float rightMovement = 0.0f;
		auto newKs = Engine::GetWindow()->GetKeyboardState();
		auto oldKs = Engine::GetWindow()->GetPreviousKeyboardState();
		if (newKs->IsKeyDown(IO::Key::W))
			forwardMovement += 0.7f;
		if (newKs->IsKeyDown(IO::Key::S))
			forwardMovement -= 0.7f;
		if (newKs->IsKeyDown(IO::Key::A))
			rightMovement -= 0.7f;
		if (newKs->IsKeyDown(IO::Key::D))
			rightMovement += 0.7f;

		if (newKs->IsKeyDown(IO::Key::LEFT_SHIFT)) {
			forwardMovement *= 0.3f;
			rightMovement *= 0.3f;
		}

		// Car
		auto& carTransform = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(ballObject);
		if (newKs->IsKeyDown(IO::Key::UP))
			carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * 2);
		if (newKs->IsKeyDown(IO::Key::DOWN))
			carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * -2);
		if (newKs->IsKeyDown(IO::Key::LEFT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
		if (newKs->IsKeyDown(IO::Key::RIGHT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, -1, 0 });

		if (newKs->IsKeyDown(IO::Key::F11) && oldKs->IsKeyUp(IO::Key::F11))
			Engine::GetWindow()->ToggleFullScreen();

		if (newKs->IsKeyDown(IO::Key::F1) && oldKs->IsKeyUp(IO::Key::F1)) {
			if (terrainMaterial == terrainMaterialFill)
				terrainMaterial = terrainMaterialLine;
			else
				terrainMaterial = terrainMaterialFill;
		}

		if (newKs->IsKeyDown(IO::Key::ESCAPE))
			this->Exit();
		if (newKs->IsKeyDown(IO::Key::P))
			this->Exit();

		camera.Rotate(
			(float)(Engine::GetWindow()->GetMouseState()->GetPosition().X - Engine::GetWindow()->GetPreviousMouseState()->GetPosition().X),
			(float)(Engine::GetWindow()->GetMouseState()->GetPosition().Y - Engine::GetWindow()->GetPreviousMouseState()->GetPosition().Y)
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

		Engine::GetEntityComponentSystem()->GetComponent<ECS::CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform2D>(cameraObject2d)
		);

		const auto& transformComponent = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(ballObject);
		auto& modelComponent = Engine::GetEntityComponentSystem()->GetComponent<ECS::ModelComponent3D>(ballObject);
		modelComponent.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent.GetAsMatrix());
		//modelComponent.GetModel()->GetAccelerationStructure()->Update();

		const auto& transformComponent2 = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(smallBallObject);
		auto& modelComponent2 = Engine::GetEntityComponentSystem()->GetComponent<ECS::ModelComponent3D>(smallBallObject);
		modelComponent2.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent2.GetAsMatrix());
		//modelComponent2.GetModel()->GetAccelerationStructure()->Update();
		//topLevelAccelerationStructure->Update();

		/*auto commandList = OSK::Engine::GetRenderer()->GetCommandList();
		commandList->TransitionImageLayout(rtTargetImage, OSK::GRAPHICS::GpuImageLayout::GENERAL, 0, 1);
		commandList->BindMaterial(rtMaterial);
		for (auto const& slotName : rtMaterialInstance->GetLayout()->GetAllSlotNames())
			commandList->BindMaterialSlot(rtMaterialInstance->GetSlot(slotName));
		commandList->TraceRays(0, 0, 0, { 1920, 1080 });
		commandList->TransitionImageLayout(rtTargetImage, OSK::GRAPHICS::GpuImageLayout::SHADER_READ_ONLY, 0, 1);/**/
	}

	void BuildFrame() override {
		auto commandList = Engine::GetRenderer()->GetCommandList();
		auto renderpass = Engine::GetRenderer()->GetFinalRenderTarget();

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

		// Render skybox
		commandList->BeginGraphicsRenderpass(&skyboxRenderTarget);
		commandList->BindMaterial(skyboxMaterial);
		commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("global"));
		commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		commandList->DrawSingleInstance(cubemapModel->GetIndexCount());
		commandList->EndGraphicsRenderpass(&skyboxRenderTarget);

		// Render text
		commandList->BeginGraphicsRenderpass(&textRenderTarget);
		spriteRenderer.Begin();
		spriteRenderer.DrawString(*font, 30, "OSKengine build " + Engine::GetBuild(), Vector2f{ 20.0f, 30.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());
		spriteRenderer.DrawString(*font, 30, "FPS " + std::to_string(GetFps()), Vector2f{ 20.0f, 60.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());
		spriteRenderer.End();
		commandList->EndGraphicsRenderpass(&textRenderTarget);

		commandList->BindMaterial(materialRenderTarget);
		commandList->BeginGraphicsRenderpass(renderpass);

		spriteRenderer.Begin();
		
		commandList->SetViewport(viewport);
		commandList->SetScissor(windowRec);

		spriteRenderer.Draw(skyboxRenderTarget.GetSprite(), skyboxRenderTarget.GetSpriteTransform());
		spriteRenderer.Draw(Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().GetSprite(), Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().GetSpriteTransform());
		//spriteRenderer.Draw(Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem2D>()->GetRenderTarget().GetSprite(), Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem2D>()->GetRenderTarget().GetSpriteTransform());
		spriteRenderer.Draw(textRenderTarget.GetSprite(), textRenderTarget.GetSpriteTransform());

		spriteRenderer.End();

		commandList->EndGraphicsRenderpass(renderpass);
	}

	void OnExit() override {
		uniformBuffer.Delete();

		delete skyboxMaterialInstance;
	}

private:

	ECS::GameObjectIndex ballObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex smallBallObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex spriteObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex terrain = ECS::EMPTY_GAME_OBJECT;

	UniquePtr<GRAPHICS::IGpuUniformBuffer> uniformBuffer;
	
	ASSETS::Texture* texture = nullptr;
	ASSETS::Font* font = nullptr;

	UI::UiRenderer uiRenderer;
	UniquePtr<UI::UiElement> mainUi;

	UniquePtr<IGpuUniformBuffer> dirLightBuffer;

};

#endif
