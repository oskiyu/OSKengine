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
#include "IKeyboardInput.h"
#include "ICommandList.h"

#include "PcUserInput.h"
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
#include "SkyboxRenderSystem.h"
#include "IGpuStorageBuffer.h"

#include "Math.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "RenderTarget.h"

#include "PbrDeferredRenderSystem.h"
#include "ModelLoader3D.h"
#include "IrradianceMap.h"

#include "FxaaPass.h"
#include "ToneMapping.h"
#include "BloomPass.h"
#include "SmaaPass.h"

OSK::GRAPHICS::Material* rtMaterial = nullptr;
OSK::GRAPHICS::MaterialInstance* rtMaterialInstance = nullptr;
OSK::GRAPHICS::GpuImage* rtTargetImage[3]{};

OSK::GRAPHICS::Material* material = nullptr;
OSK::GRAPHICS::Material* material2d = nullptr;
OSK::GRAPHICS::Material* materialRenderTarget = nullptr;

OSK::GRAPHICS::SpriteRenderer spriteRenderer;

OSK::GRAPHICS::ITopLevelAccelerationStructure* topLevelAccelerationStructure = nullptr;
OSK::GRAPHICS::IGpuStorageBuffer* instancesInfoBuffer = nullptr;

OSK::GRAPHICS::RenderTarget textRenderTarget;

struct RtInstanceInfo {
	TSize vertexOffset = 0; // In bytes
	TSize indexOffset = 0; // In bytes
};

// PBR
using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
Material* pbrColorMaterial = nullptr;
Material* pbrNormalMaterial = nullptr;

#define OSK_CURRENT_RSYSTEM OSK::ECS::HybridRenderSystem

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		Engine::GetDisplay()->Create({ 1280u, 720u }, "OSKengine");

		IO::IMouseInput* mouseInput = nullptr;
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouseInput);

		mouseInput->SetReturnMode(IO::MouseReturnMode::ALWAYS_RETURN);
		mouseInput->SetMotionMode(IO::MouseMotionMode::RAW);
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetDisplay(), PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
		auto irradianceMap = Engine::GetAssetManager()->Load<ASSETS::IrradianceMap>("Resources/Assets/IBL/irradiance0.json", "GLOBAL");
		auto animModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/animmodel.json", "GLOBAL");
		
		// Material load
		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_pbr.json"); //Resources/PbrMaterials/deferred_gbuffer.json
		material2d = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json");
		materialRenderTarget = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rendertarget.json");

		texture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/texture0.json", "GLOBAL");

		font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		ASSETS::Model3D* carModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/f1.json", "GLOBAL");
		ASSETS::Model3D* circuitModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/circuit0.json", "GLOBAL");

		topLevelAccelerationStructure = Engine::GetRenderer()->GetAllocator()->CreateTopAccelerationStructure({
			carModel->GetAccelerationStructure(),
			//circuitModel->GetAccelerationStructure()
			}).GetPointer();

		DynamicArray<RtInstanceInfo> instancesInfo;
		instancesInfo.Insert({
			carModel->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - carModel->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			carModel->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - carModel->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			});
		//instancesInfo.Insert({ 
			//circuitModel->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - circuitModel->GetVertexBuffer()->GetMemorySubblock()->GetOffsetFromBlock(),
			//circuitModel->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock() - circuitModel->GetIndexBuffer()->GetMemorySubblock()->GetOffsetFromBlock()
			//});
		instancesInfoBuffer = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(RtInstanceInfo) * 4).GetPointer();
		instancesInfoBuffer->MapMemory();
		instancesInfoBuffer->Write(instancesInfo.GetData(), instancesInfo.GetSize() * sizeof(RtInstanceInfo));
		instancesInfoBuffer->Unmap();/**/

		for (TSize i = 0; i < _countof(rtTargetImage); i++) {
			GpuImageSamplerDesc sampler{};
			sampler.mipMapMode = GpuImageMipmapMode::NONE;
			rtTargetImage[i] = Engine::GetRenderer()->GetAllocator()->CreateImage({ 1920, 1080, 1 }, GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT,
				GpuImageUsage::RT_TARGET_IMAGE | GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
			rtTargetImage[i]->SetDebugName("RtTargetImage [" + std::to_string(i) + "]");
		}

		OSK_CURRENT_RSYSTEM* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		rtMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rt.json");
		rtMaterialInstance = rtMaterial->CreateInstance().GetPointer();
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("vertices", carModel->GetVertexBuffer());
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("indices", carModel->GetIndexBuffer());
		rtMaterialInstance->GetSlot("rt")->SetStorageBuffer("instanceInfos", instancesInfoBuffer);
		rtMaterialInstance->GetSlot("rt")->SetAccelerationStructure("topLevelAccelerationStructure", topLevelAccelerationStructure);

		renderSystem->AddBlas(carModel->GetAccelerationStructure());
		//renderSystem->AddBlas(circuitModel->GetAccelerationStructure());

		const IGpuUniformBuffer* cameraBuffers[3] {
			renderSystem->GetCameraBuffer(0),
			renderSystem->GetCameraBuffer(1),
			renderSystem->GetCameraBuffer(2)
		};
		const GpuImage* rtImages[3] {
			rtTargetImage[0],
			rtTargetImage[1],
			rtTargetImage[2]
		};

		rtMaterialInstance->GetSlot("rt")->SetStorageImages("targetImage", rtImages);
		rtMaterialInstance->GetSlot("rt")->FlushUpdate();
		rtMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraBuffers);
		rtMaterialInstance->GetSlot("global")->FlushUpdate();

		// ECS
		carObject = Engine::GetEcs()->SpawnObject();

		cameraObject = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<ECS::CameraComponent3D>(cameraObject, {});
		auto& cameraTransform = Engine::GetEcs()->AddComponent<ECS::Transform3D>(cameraObject, ECS::Transform3D(cameraObject));
		cameraTransform.AddPosition({ 0.0f, 0.3f, 0.0f });
		renderSystem->Initialize(cameraObject, *irradianceMap);
		Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->SetCamera(cameraObject);

		ECS::Transform3D& transform = Engine::GetEcs()->AddComponent<ECS::Transform3D>(carObject, ECS::Transform3D(carObject));
		ECS::ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ECS::ModelComponent3D>(carObject, {});
		
		cameraTransform.AttachToObject(carObject);

		modelComponent->SetModel(carModel);
		modelComponent->SetMaterial(material);
		modelComponent->BindTextureForAllMeshes("texture", "albedoTexture", texture);
		ModelLoader3D::SetupPbrModel(carModel, modelComponent);

		// ECS 2
		circuitObject = Engine::GetEcs()->SpawnObject();

		auto& transform2 = Engine::GetEcs()->AddComponent<ECS::Transform3D>(circuitObject, ECS::Transform3D(circuitObject));
		/*auto modelComponent2 = &Engine::GetEcs()->AddComponent<ECS::ModelComponent3D>(circuitObject, {});

		modelComponent2->SetModel(circuitModel); // animModel
		modelComponent2->SetMaterial(material);
		modelComponent2->BindTextureForAllMeshes("texture", "albedoTexture", texture);
		ModelLoader3D::SetupPbrModel(circuitModel, modelComponent2);*/
		// circuitModel->GetAnimator()->AddActiveAnimation("Idle");
		// circuitModel->GetAnimator()->AddActiveAnimation("Run");

		// Cubemap
		Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->SetCubemap(*Engine::GetAssetManager()->Load<ASSETS::CubemapTexture>("Resources/Assets/skybox0.json", "GLOBAL"));
		
		cameraObject2d = Engine::GetEcs()->SpawnObject();
		auto& camera2D = Engine::GetEcs()->AddComponent<ECS::CameraComponent2D>(cameraObject2d, {});
		Engine::GetEcs()->AddComponent<ECS::Transform2D>(cameraObject2d, { cameraObject2d });
		camera2D.LinkToDisplay(Engine::GetDisplay());

		spriteRenderer.SetCommandList(Engine::GetRenderer()->GetGraphicsCommandList());

		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->SetUniformBuffer("camera", camera2D.GetUniformBuffer());
		font->GetInstance(30).sprite->GetMaterialInstance()->GetSlot("global")->FlushUpdate();

		// Terrain
		// renderSystem->InitializeTerrain({ 10u }, *Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/heightmap0.json", "GLOBAL"), *Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/terrain0.json", "GLOBAL"));

		RenderTargetAttachmentInfo textColorInfo{ .format = Format::RGBA8_UNORM, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE };
		RenderTargetAttachmentInfo textDepthInfo{ .format = Format::D32S8_SFLOAT_SUINT };
		textRenderTarget.Create(Engine::GetDisplay()->GetResolution(), { textColorInfo }, textDepthInfo);

		RenderTargetAttachmentInfo preEffectsColorInfo{ .format = Format::RGBA32_SFLOAT, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE };
		RenderTargetAttachmentInfo preEffectsDepthInfo{ .format = Format::D32S8_SFLOAT_SUINT };
		preEffectsRenderTarget.Create(Engine::GetDisplay()->GetResolution(), { preEffectsColorInfo }, preEffectsDepthInfo);
		

		fxaaPass = new FxaaPass();
		fxaaPass->Create(Engine::GetDisplay()->GetResolution());

		bloomPass = new BloomPass();
		bloomPass->Create(Engine::GetDisplay()->GetResolution());

		toneMappingPass = new ToneMappingPass();
		toneMappingPass->Create(Engine::GetDisplay()->GetResolution());

		SetupPostProcessingChain();

		const IGpuStorageBuffer* epxBuffers[3]{};
		for (TSize i = 0; i < _countof(exposureBuffers); i++) {
			exposureBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(float)).GetPointer();
			epxBuffers[i] = exposureBuffers[i].GetPointer();

			exposureBuffers[i]->MapMemory();
			exposureBuffers[i]->Write(toneMappingPass->GetExposure());
			exposureBuffers[i]->Unmap();
		}

		toneMappingPass->SetExposureBuffers(epxBuffers);

		Engine::GetRenderer()->RegisterRenderTarget(&textRenderTarget);
	}

	void RegisterSystems() override {
		Engine::GetEcs()->RemoveSystem<ECS::RenderSystem2D>();
	}

	void OnTick(TDeltaTime deltaTime) override {
		ECS::CameraComponent3D& camera = Engine::GetEcs()->GetComponent<ECS::CameraComponent3D>(cameraObject);
		ECS::Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<ECS::Transform3D>(cameraObject);

		float forwardMovement = 0.0f;
		float rightMovement = 0.0f;

		const IO::IKeyboardInput* keyboard = nullptr;
		const IO::IGamepadInput* gamepad = nullptr;
		const IO::IMouseInput* mouse = nullptr;

		Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);
		Engine::GetInput()->QueryInterface(IUUID::IGamepadInput, (void**)&gamepad);
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouse);

		if (keyboard->IsKeyUp(IO::Key::F1)) {
			toneMappingPass->SetExposure(toneMappingPass->GetExposure() - deltaTime * 2);
			for (TSize i = 0; i < _countof(exposureBuffers); i++) {
				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}
		}
		if (keyboard->IsKeyUp(IO::Key::F2)) {
			toneMappingPass->SetExposure(toneMappingPass->GetExposure() + deltaTime * 2);
			for (TSize i = 0; i < _countof(exposureBuffers); i++) {
				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}
		}

		if (keyboard->IsKeyUp(IO::Key::J))
			toneMappingPass->SetGamma(toneMappingPass->GetGamma() - deltaTime * 2);
		if (keyboard->IsKeyUp(IO::Key::K))
			toneMappingPass->SetGamma(toneMappingPass->GetGamma() + deltaTime * 2);

		if (keyboard->IsKeyDown(IO::Key::W))
			forwardMovement += 0.7f;
		if (keyboard->IsKeyDown(IO::Key::S))
			forwardMovement -= 0.7f;
		if (keyboard->IsKeyDown(IO::Key::A))
			rightMovement -= 0.7f;
		if (keyboard->IsKeyDown(IO::Key::D))
			rightMovement += 0.7f;

		Vector2f cameraRotation = {
			(float)(mouse->GetMouseState().GetPosition().X - mouse->GetPreviousMouseState().GetPosition().X),
			(float)(mouse->GetMouseState().GetPosition().Y - mouse->GetPreviousMouseState().GetPosition().Y)
		};

		const IO::GamepadState& gamepadState = gamepad->GetGamepadState(0);
		if (gamepadState.IsConnected()) {
			forwardMovement -= gamepadState.GetAxisState(IO::GamepadAxis::LEFT_Y);

			rightMovement += gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X);

			cameraRotation.X += gamepadState.GetAxisState(IO::GamepadAxis::RIGHT_X);
			cameraRotation.Y += gamepadState.GetAxisState(IO::GamepadAxis::RIGHT_Y);
		}

		if (keyboard->IsKeyDown(IO::Key::LEFT_SHIFT)) {
			forwardMovement *= 0.3f;
			rightMovement *= 0.3f;
		}

		// Car
		static float carSpeed = 0.0f;
		float speedDiff = 0.0f;
		auto& carTransform = Engine::GetEcs()->GetComponent<ECS::Transform3D>(carObject);
		if (keyboard->IsKeyDown(IO::Key::UP))
			speedDiff += 0.5f * deltaTime;
		if (keyboard->IsKeyDown(IO::Key::DOWN))
			speedDiff -= 0.5f * deltaTime;
		carSpeed += speedDiff;
		carSpeed = OSK::MATH::Clamp(carSpeed, -1.0f, 7.0f);
		carTransform.AddPosition(carTransform.GetForwardVector() * deltaTime * carSpeed);
		if (keyboard->IsKeyDown(IO::Key::LEFT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
		if (keyboard->IsKeyDown(IO::Key::RIGHT))
			carTransform.RotateWorldSpace(deltaTime * 2, { 0, -1, 0 });

		if (keyboard->IsKeyStroked(IO::Key::F11)) {
			IO::IFullscreenableDisplay* display = nullptr;
			Engine::GetDisplay()->QueryInterface(OSK_IUUID(IO::IFullscreenableDisplay), (void**)&display);

			display->ToggleFullscreen();
		}

		if (keyboard->IsKeyDown(IO::Key::ESCAPE))
			this->Exit();
		if (keyboard->IsKeyDown(IO::Key::P))
			this->Exit();

		camera.Rotate(cameraRotation.X, cameraRotation.Y);

		cameraTransform.AddPosition(cameraTransform.GetForwardVector().GetNormalized() * forwardMovement * deltaTime);
		cameraTransform.AddPosition(cameraTransform.GetRightVector().GetNormalized() * rightMovement * deltaTime);
		camera.UpdateTransform(&cameraTransform);

		Engine::GetEcs()->GetComponent<ECS::CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			Engine::GetEcs()->GetComponent<ECS::Transform2D>(cameraObject2d)
		);

		if (OSK::Engine::GetRenderer()->IsRtActive()) {
			auto commandList = OSK::Engine::GetRenderer()->GetGraphicsCommandList();

			const auto& transformComponent = Engine::GetEcs()->GetComponent<ECS::Transform3D>(carObject);
			auto& modelComponent = Engine::GetEcs()->GetComponent<ECS::ModelComponent3D>(carObject);
			modelComponent.GetModel()->GetAccelerationStructure()->SetMatrix(transformComponent.GetAsMatrix());
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
			Engine::GetDisplay()->GetResolution().X,
			Engine::GetDisplay()->GetResolution().Y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		graphicsCommandList->SetViewport(viewport);
		graphicsCommandList->SetScissor(windowRec);

		// Render text
		graphicsCommandList->BeginGraphicsRenderpass(&textRenderTarget, Color::BLACK() * 0.0f);
		spriteRenderer.Begin();
		spriteRenderer.DrawString(*font, 30, "OSKengine build " + Engine::GetBuild(), Vector2f{ 20.0f, 30.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());
		spriteRenderer.DrawString(*font, 30, "FPS " + std::to_string(GetFps()), Vector2f{ 20.0f, 60.0f }, Vector2f{ 1.0f }, 0.0f, Color::WHITE());

		spriteRenderer.End();
		graphicsCommandList->EndGraphicsRenderpass();

		// Full-screen rendering
		//
		// Pre-Effects
		graphicsCommandList->BindVertexBuffer(Sprite::globalVertexBuffer);
		graphicsCommandList->BindIndexBuffer(Sprite::globalIndexBuffer);

		graphicsCommandList->BeginGraphicsRenderpass(&preEffectsRenderTarget);
		graphicsCommandList->BindMaterial(Engine::GetRenderer()->GetFullscreenRenderingMaterial());

		graphicsCommandList->BindMaterialSlot(Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
		graphicsCommandList->DrawSingleInstance(6);

		graphicsCommandList->BindMaterialSlot(Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>()->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
		graphicsCommandList->DrawSingleInstance(6);

		graphicsCommandList->EndGraphicsRenderpass();

		// Post-processing effects:
		fxaaPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		bloomPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		toneMappingPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());

		// Frame build:
		frameBuildCommandList->BindVertexBuffer(Sprite::globalVertexBuffer);
		frameBuildCommandList->BindIndexBuffer(Sprite::globalIndexBuffer);

		frameBuildCommandList->SetViewport(viewport);
		frameBuildCommandList->SetScissor(windowRec);

		frameBuildCommandList->BeginGraphicsRenderpass(renderpass);
		frameBuildCommandList->BindMaterial(Engine::GetRenderer()->GetFullscreenRenderingMaterial());

		frameBuildCommandList->BindMaterialSlot(toneMappingPass->GetOutput().GetFullscreenSpriteMaterialSlot());
		frameBuildCommandList->DrawSingleInstance(6);

		frameBuildCommandList->BindMaterialSlot(textRenderTarget.GetFullscreenSpriteMaterialSlot());
		frameBuildCommandList->DrawSingleInstance(6);

		frameBuildCommandList->EndGraphicsRenderpass();
	}

	void SetupPostProcessingChain() {
		fxaaPass->SetInput(preEffectsRenderTarget, IPostProcessPass::InputType::SAMPLER);
		bloomPass->SetInput(fxaaPass->GetOutput(), IPostProcessPass::InputType::SAMPLER);
		toneMappingPass->SetInput(bloomPass->GetOutput(), IPostProcessPass::InputType::SAMPLER);

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

	}

private:

	RenderTarget preEffectsRenderTarget;
	UniquePtr<BloomPass> bloomPass;
	UniquePtr<FxaaPass> fxaaPass;
	UniquePtr<SmaaPass> smaaPass;
	UniquePtr<ToneMappingPass> toneMappingPass;

	UniquePtr<IGpuStorageBuffer> exposureBuffers[3];

	ECS::GameObjectIndex carObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex circuitObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;

	ASSETS::Texture* texture = nullptr;
	ASSETS::Font* font = nullptr;

	UI::UiRenderer uiRenderer;
	UniquePtr<UI::UiElement> mainUi;

};

#endif
