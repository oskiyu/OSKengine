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

#include "Math.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "RenderTarget.h"

#include "IrradianceMap.h"
#include "IrradianceMapLoader.h"

#include "FxaaPass.h"
#include "ToneMapping.h"
#include "BloomPass.h"

OSK::GRAPHICS::Material* rtMaterial = nullptr;
OSK::GRAPHICS::MaterialInstance* rtMaterialInstance = nullptr;
OSK::GRAPHICS::GpuImage* rtTargetImage[3]{};

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
OSK::GRAPHICS::IGpuStorageBuffer* instancesInfoBuffer = nullptr;

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
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetWindow(), PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
		// Material load

		auto irradianceMap = Engine::GetAssetManager()->Load<ASSETS::IrradianceMap>("Resources/Assets/IBL/irradiance0.json", "GLOBAL");

		Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_irradiance_gen.json");
		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_pbr.json");
		skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
		material2d = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json");
		materialRenderTarget = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rendertarget.json");
		computeMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/compute0.json"); 
		computeMaterialInstance = computeMaterial->CreateInstance().GetPointer();

		texture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		const IGpuUniformBuffer* cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
			uniformBuffer[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
			cameraUbos[i] = uniformBuffer[i].GetPointer();
		}
		
		ASSETS::Model3D* model = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		ASSETS::Model3D* model_low = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/circuit0.json", "GLOBAL");

		//topLevelAccelerationStructure = Engine::GetRenderer()->GetMemoryAllocator()->CreateTopAccelerationStructure({
		//	model->GetAccelerationStructure(),
		//	model_low->GetAccelerationStructure()
		//	}).GetPointer();

		DynamicArray<RtInstanceInfo> instancesInfo;
		instancesInfo.Insert({
			model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfo.Insert({ 
			model_low->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			model_low->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - model->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		instancesInfoBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateStorageBuffer(sizeof(RtInstanceInfo) * 4).GetPointer();
		instancesInfoBuffer->MapMemory();
		instancesInfoBuffer->Write(instancesInfo.GetData(), instancesInfo.GetSize() * sizeof(RtInstanceInfo));
		instancesInfoBuffer->Unmap();/**/

		for (TSize i = 0; i < _countof(rtTargetImage); i++) {
			GpuImageSamplerDesc sampler{};
			sampler.mipMapMode = GpuImageMipmapMode::NONE;
			rtTargetImage[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ 1920, 1080, 1 }, GpuImageDimension::d2D, 1, Format::RGBA8_UNORM,
				GpuImageUsage::RT_TARGET_IMAGE | GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
			rtTargetImage[i]->SetDebugName("RtTargetImage [" + std::to_string(i) + "]");
		}

		/*rtMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rt.json");
		rtMaterialInstance = rtMaterial->CreateInstance().GetPointer();
//		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("vertices", model->GetVertexBuffer());
//		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("indices", model->GetIndexBuffer());
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("instanceInfos", instancesInfoBuffer);
//		rtMaterialInstance->GetSlot("rt")->SetAccelerationStructure("topLevelAccelerationStructure", topLevelAccelerationStructure);
		
		rtMaterialInstance->GetSlot("rt")->SetStorageImages("targetImage", imgs);
		rtMaterialInstance->GetSlot("rt")->FlushUpdate();
		rtMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
		rtMaterialInstance->GetSlot("global")->FlushUpdate();*/
		const GpuImage* imgs[3] = { rtTargetImage[0], rtTargetImage[1], rtTargetImage[2] };
		computeMaterialInstance->GetSlot("texture")->SetStorageImages("image", imgs);
		computeMaterialInstance->GetSlot("texture")->FlushUpdate();


		// ECS
		ballObject = Engine::GetEntityComponentSystem()->SpawnObject();

		ECS::Transform3D& transform = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(ballObject, ECS::Transform3D(ballObject));
		ECS::ModelComponent3D* modelComponent = &Engine::GetEntityComponentSystem()->AddComponent<ECS::ModelComponent3D>(ballObject, {});
				
		DirectionalLight dirLight;
		const Vector3f direction = Vector3f(1.0f, -3.f, 0.0f).GetNormalized();
		dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.2f);
		dirLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);

		ECS::RenderSystem3D* renderSystem = Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>();

		modelComponent->SetModel(model);
		modelComponent->SetMaterial(material);

		modelComponent->BindTextureForAllMeshes("model", "stexture", texture);
		modelComponent->BindGpuImageForAllMeshes("global", "irradianceMap", irradianceMap->GetGpuImage());
		modelComponent->BindUniformBufferForAllMeshes("global", "dirLight", renderSystem->GetDirLightUniformBuffer());

		const IGpuUniformBuffer* shadowsMatUbos[NUM_RESOURCES_IN_FLIGHT]{};
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
			shadowsMatUbos[i] = renderSystem->GetShadowMap()->GetDirLightMatrixUniformBuffers().At(i);

		for (TSize i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& metadata = model->GetMetadata().meshesMetadata[i];

			const GpuImage* images[3]{};
			for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
				images[i] = renderSystem->GetShadowMap()->GetShadowImage(i);

			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImages("dirLightShadowMap", images, SampledChannel::DEPTH, SampledArrayType::ARRAY);
			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", shadowsMatUbos);
			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
			modelComponent->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent->GetMeshMaterialInstance(i)->GetSlot("model")->SetGpuImage("stexture", model->GetImage(texture.second));

				modelComponent->GetMeshMaterialInstance(i)->GetSlot("model")->FlushUpdate();
			}
		}

		cameraObject = Engine::GetEntityComponentSystem()->SpawnObject();

		Engine::GetEntityComponentSystem()->AddComponent<ECS::CameraComponent3D>(cameraObject, {});
		auto& cameraTransform = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(cameraObject, ECS::Transform3D(cameraObject));
		cameraTransform.AddPosition({ 0.0f, 0.3f, 0.0f });
		cameraTransform.AttachToObject(ballObject);
		renderSystem->GetShadowMap()->SetSceneCamera(cameraObject);
		renderSystem->SetDirectionalLight(dirLight);

		// ECS 2
		smallBallObject = Engine::GetEntityComponentSystem()->SpawnObject();

		auto& transform2 = Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform3D>(smallBallObject, ECS::Transform3D(smallBallObject));
		auto modelComponent2 = &Engine::GetEntityComponentSystem()->AddComponent<ECS::ModelComponent3D>(smallBallObject, {});

		modelComponent2->SetModel(model_low);
		modelComponent2->SetMaterial(material);

		modelComponent2->BindTextureForAllMeshes("model", "stexture", texture);
		modelComponent2->BindGpuImageForAllMeshes("global", "irradianceMap", irradianceMap->GetGpuImage());
		modelComponent2->BindUniformBufferForAllMeshes("global", "dirLight", renderSystem->GetDirLightUniformBuffer());

		for (TSize i = 0; i < model_low->GetMeshes().GetSize(); i++) {
			auto& metadata = model_low->GetMetadata().meshesMetadata[i];
			
			const GpuImage* images[3]{};
			for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
				images[i] = renderSystem->GetShadowMap()->GetShadowImage(i);

			modelComponent2->GetMeshMaterialInstance(i)->GetSlot("global")->SetGpuImages("dirLightShadowMap", images, SampledChannel::DEPTH, SampledArrayType::ARRAY);
			modelComponent2->GetMeshMaterialInstance(i)->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
			modelComponent2->GetMeshMaterialInstance(i)->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", shadowsMatUbos);
			modelComponent2->GetMeshMaterialInstance(i)->GetSlot("global")->FlushUpdate();

			if (metadata.materialTextures.GetSize() > 0) {
				for (auto& texture : metadata.materialTextures)
					modelComponent2->GetMeshMaterialInstance(i)->GetSlot("model")->SetGpuImage("stexture", model_low->GetImage(texture.second));

				modelComponent2->GetMeshMaterialInstance(i)->GetSlot("model")->FlushUpdate();
			}
		}

		// Cubemap
		cubemap = Engine::GetAssetManager()->Load<ASSETS::CubemapTexture>("Resources/Assets/skybox0.json", "GLOBAL");
		cubemapModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/cube.json", "GLOBAL");

		skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();
		skyboxMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
		skyboxMaterialInstance->GetSlot("global")->SetGpuImage("skybox", cubemap->GetGpuImage());
		skyboxMaterialInstance->GetSlot("global")->FlushUpdate();

		cameraObject2d = Engine::GetEntityComponentSystem()->SpawnObject();
		auto& camera2D = Engine::GetEntityComponentSystem()->AddComponent<ECS::CameraComponent2D>(cameraObject2d, {});
		Engine::GetEntityComponentSystem()->AddComponent<ECS::Transform2D>(cameraObject2d, { cameraObject2d });
		camera2D.LinkToWindow(Engine::GetWindow());

		spriteObject = Engine::GetEntityComponentSystem()->SpawnObject();

		spriteRenderer.SetCommandList(Engine::GetRenderer()->GetGraphicsCommandList());

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

		terrainComponent.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
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

		depthImageSprite.SetMaterialInstance(material2d->CreateInstance().GetPointer());
		depthImageSprite.SetCamera(camera2D);
		const GpuImage* depthImages[3]{nullptr};
		for (TSize i = 0; i < 3; i++)
			depthImages[i] = renderSystem->GetShadowMap()->GetShadowImage(i);
		depthImageSprite.GetMaterialInstance()->GetSlot("texture")->SetGpuImages("stexture", imgs);
		depthImageSprite.GetMaterialInstance()->GetSlot("texture")->FlushUpdate();

		depthImageSpriteTransform.SetScale({ 200.0f, 200.0f });

		ECS::Transform2D depthImageSpriteTransform{ ECS::EMPTY_GAME_OBJECT };

		skyboxRenderTarget.Create(Engine::GetWindow()->GetWindowSize(), Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);
		textRenderTarget.Create(Engine::GetWindow()->GetWindowSize(), Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);
		preEffectsRenderTarget.SetTargetImageUsage(GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE);
		preEffectsRenderTarget.Create(Engine::GetWindow()->GetWindowSize(), Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);

		fxaaPass = new FxaaPass();
		fxaaPass->Create(Engine::GetWindow()->GetWindowSize());

		bloomPass = new BloomPass();
		bloomPass->Create(Engine::GetWindow()->GetWindowSize());

		toneMappingPass = new ToneMappingPass();
		toneMappingPass->Create(Engine::GetWindow()->GetWindowSize());

		SetupPostProcessingChain();

		const IGpuStorageBuffer* epxBuffers[3]{};
		for (TSize i = 0; i < _countof(exposureBuffers); i++) {
			exposureBuffers[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateStorageBuffer(sizeof(float)).GetPointer();
			epxBuffers[i] = exposureBuffers[i].GetPointer();

			exposureBuffers[i]->MapMemory();
			exposureBuffers[i]->Write(toneMappingPass->GetExposure());
			exposureBuffers[i]->Unmap();
		}

		bloomPass->SetExposureBuffers(epxBuffers);
		toneMappingPass->SetExposureBuffers(epxBuffers);

		Engine::GetRenderer()->RegisterRenderTarget(&skyboxRenderTarget);
		Engine::GetRenderer()->RegisterRenderTarget(&textRenderTarget);

		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetShadowMap()->SetLightOrigin(cameraTransform.GetPosition()* Vector3f { 1.0f, 0.0f, 1.0f });
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

		if (newKs->IsKeyUp(IO::Key::F1)) {
			toneMappingPass->SetExposure(toneMappingPass->GetExposure() - deltaTime * 2);
			for (TSize i = 0; i < _countof(exposureBuffers); i++) {
				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}
		}
		if (newKs->IsKeyUp(IO::Key::F2)) {
			toneMappingPass->SetExposure(toneMappingPass->GetExposure() + deltaTime * 2);
			for (TSize i = 0; i < _countof(exposureBuffers); i++) {
				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}
		}

		if (newKs->IsKeyUp(IO::Key::J))
			toneMappingPass->SetGamma(toneMappingPass->GetGamma() - deltaTime * 2);
		if (newKs->IsKeyUp(IO::Key::K))
			toneMappingPass->SetGamma(toneMappingPass->GetGamma() + deltaTime * 2);

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
		static float carSpeed = 0.0f;
		float speedDiff = 0.0f;
		auto& carTransform = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(ballObject);
		if (newKs->IsKeyDown(IO::Key::UP))
			speedDiff += 0.5f * deltaTime;
		if (newKs->IsKeyDown(IO::Key::DOWN))
			speedDiff -= 0.5f * deltaTime;
		carSpeed += speedDiff;
		carSpeed = OSK::MATH::Clamp(carSpeed, -1.0f, 7.0f);
		carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * carSpeed);
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

		const TSize frameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();
		uniformBuffer[frameIndex]->ResetCursor();
		uniformBuffer[frameIndex]->MapMemory();
		uniformBuffer[frameIndex]->Write(camera.GetProjectionMatrix());
		uniformBuffer[frameIndex]->Write(camera.GetViewMatrix(cameraTransform));
		uniformBuffer[frameIndex]->Write(cameraTransform.GetPosition());
		uniformBuffer[frameIndex]->Unmap();

		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetShadowMap()->SetLightOrigin(cameraTransform.GetPosition() * Vector3f{ 1.0f, 0.0f, 1.0f});

		Engine::GetEntityComponentSystem()->GetComponent<ECS::CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform2D>(cameraObject2d)
		);

		if (OSK::Engine::GetRenderer()->IsRtActive()) {
			const auto& transformComponent = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(ballObject);
			auto& modelComponent = Engine::GetEntityComponentSystem()->GetComponent<ECS::ModelComponent3D>(ballObject);
			modelComponent.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent.GetAsMatrix());
			modelComponent.GetModel()->GetAccelerationStructure()->Update();

			const auto& transformComponent2 = Engine::GetEntityComponentSystem()->GetComponent<ECS::Transform3D>(smallBallObject);
			auto& modelComponent2 = Engine::GetEntityComponentSystem()->GetComponent<ECS::ModelComponent3D>(smallBallObject);
			modelComponent2.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent2.GetAsMatrix());
			modelComponent2.GetModel()->GetAccelerationStructure()->Update();
			topLevelAccelerationStructure->Update();

			const TSize imgIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();
			auto commandList = OSK::Engine::GetRenderer()->GetGraphicsCommandList();

			commandList->SetGpuImageBarrier(rtTargetImage[imgIndex], GpuImageLayout::SHADER_READ_ONLY, GpuImageLayout::GENERAL,
				GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

			commandList->BindMaterial(rtMaterial);
			for (auto const& slotName : rtMaterialInstance->GetLayout()->GetAllSlotNames())
				commandList->BindMaterialSlot(rtMaterialInstance->GetSlot(slotName));
			commandList->TraceRays(0, 0, 0, { 1920, 1080 });

			commandList->SetGpuImageBarrier(rtTargetImage[imgIndex], GpuImageLayout::GENERAL, GpuImageLayout::SHADER_READ_ONLY,
				GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));
		}
	}

	void BuildFrame() override {
		auto graphicsCommandList = Engine::GetRenderer()->GetGraphicsCommandList();
		auto frameBuildCommandList = Engine::GetRenderer()->GetFrameBuildCommandList();
		auto renderpass = Engine::GetRenderer()->GetFinalRenderTarget();

		static SpriteRenderer spriteRenderer{};
		spriteRenderer.SetCommandList(graphicsCommandList);

		const Vector4ui windowRec = {
			0,
			0,
			Engine::GetWindow()->GetWindowSize().X,
			Engine::GetWindow()->GetWindowSize().Y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		graphicsCommandList->SetViewport(viewport);
		graphicsCommandList->SetScissor(windowRec);

		// Render skybox
		graphicsCommandList->BeginGraphicsRenderpass(&skyboxRenderTarget, Color::BLACK() * 0.0f);
		graphicsCommandList->BindMaterial(skyboxMaterial);
		graphicsCommandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("global"));
		graphicsCommandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
		graphicsCommandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
		graphicsCommandList->PushMaterialConstants("brightness", 1.0f /*Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetDirectionalLight().directionAndIntensity.W*/);
		graphicsCommandList->DrawSingleInstance(cubemapModel->GetIndexCount());
		graphicsCommandList->EndGraphicsRenderpass();

		// Render text
		graphicsCommandList->BeginGraphicsRenderpass(&textRenderTarget, Color::BLACK() * 0.0f);
		spriteRenderer.Begin();
		spriteRenderer.DrawString(*font, 30, "OSKengine build " + Engine::GetBuild(), Vector2f{ 20.0f, 30.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());
		spriteRenderer.DrawString(*font, 30, "FPS " + std::to_string(GetFps()), Vector2f{ 20.0f, 60.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());

		spriteRenderer.End();
		graphicsCommandList->EndGraphicsRenderpass();

		// Pre-Effects
		graphicsCommandList->BindMaterial(material2d);
		graphicsCommandList->BeginGraphicsRenderpass(&preEffectsRenderTarget);
		spriteRenderer.Begin();
		spriteRenderer.Draw(skyboxRenderTarget.GetSprite(), skyboxRenderTarget.GetSpriteTransform());
		spriteRenderer.Draw(Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().GetSprite(), Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->GetRenderTarget().GetSpriteTransform());
		spriteRenderer.End();
		graphicsCommandList->EndGraphicsRenderpass();

		fxaaPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		bloomPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		toneMappingPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());

		frameBuildCommandList->BeginGraphicsRenderpass(renderpass);

		spriteRenderer.SetCommandList(frameBuildCommandList);
		spriteRenderer.Begin();
		
		frameBuildCommandList->SetViewport(viewport);
		frameBuildCommandList->SetScissor(windowRec);

		spriteRenderer.Draw(toneMappingPass->GetOutput().GetSprite(), toneMappingPass->GetOutput().GetSpriteTransform());
		spriteRenderer.Draw(textRenderTarget.GetSprite(), textRenderTarget.GetSpriteTransform());

		spriteRenderer.End();

		frameBuildCommandList->EndGraphicsRenderpass();
	}

	void SetupPostProcessingChain() {
		fxaaPass->SetInput(preEffectsRenderTarget);
		bloomPass->SetInput(fxaaPass->GetOutput());
		toneMappingPass->SetInput(bloomPass->GetOutput());

		fxaaPass->UpdateMaterialInstance();
		bloomPass->UpdateMaterialInstance();
		toneMappingPass->UpdateMaterialInstance();
	}

	void OnWindowResize(const Vector2ui& size) {
		preEffectsRenderTarget.Resize(size);
		fxaaPass->Resize(size);
		bloomPass->Resize(size);
		toneMappingPass->Resize(size);

		SetupPostProcessingChain();
	}

	void OnExit() override {
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
			uniformBuffer[i].Delete();

		delete skyboxMaterialInstance;
	}

private:

	RenderTarget preEffectsRenderTarget;
	UniquePtr<BloomPass> bloomPass;
	UniquePtr<FxaaPass> fxaaPass;
	UniquePtr<ToneMappingPass> toneMappingPass;

	UniquePtr<IGpuStorageBuffer> exposureBuffers[3];

	ECS::GameObjectIndex ballObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex smallBallObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex spriteObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex terrain = ECS::EMPTY_GAME_OBJECT;

	UniquePtr<GRAPHICS::IGpuUniformBuffer> uniformBuffer[NUM_RESOURCES_IN_FLIGHT];

	Material* computeMaterial = nullptr;
	UniquePtr<MaterialInstance> computeMaterialInstance = nullptr;
	
	ASSETS::Texture* texture = nullptr;
	ASSETS::Font* font = nullptr;

	Sprite depthImageSprite{};
	ECS::Transform2D depthImageSpriteTransform{ ECS::EMPTY_GAME_OBJECT };

	UI::UiRenderer uiRenderer;
	UniquePtr<UI::UiElement> mainUi;

};

#endif
