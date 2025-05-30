#pragma once

// import OSKengine.Editor.Ui;

#include "ApiCall.h"

#ifdef OSK_DEVELOPMENT

#include "IDebugGame.h"
#include "OSKengine.h"
#include "Window.h"
#include "IRenderer.h"
#include "GameObject.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Texture.h"
#include "AssetManager.h"
#include "IMaterialSlot.h"
#include "GpuBuffer.h"
#include "UniquePtr.hpp"
#include "IGpuMemoryAllocator.h"
#include "EntityComponentSystem.h"
#include "TransformComponent3D.h"
#include "TransformComponent2D.h"
#include "Model3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"
#include "CameraComponent2D.h"
#include "KeyboardState.h"
#include "Sprite.h"
#include "MouseState.h"
#include "MouseModes.h"
#include "CubemapTexture.h"
#include "IKeyboardInput.h"
#include "ICommandList.h"

#include "UiImageView.h"
#include "UiTextView.h"
#include "UiFreeContainer.h"
#include "UiHorizontalContainer.h"
#include "UiVerticalContainer.h"
#include "UiButton.h"
#include "UiDropdown.h"
#include "UiTextInput.h"
#include "UiCollapsibleWrapper.h"

#include "AssetLoaderJob.h"

#include "PcUserInput.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "Vertex2D.h"
#include "Sprite.h"
#include "PushConst2D.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "Font.h"
#include "FontLoader.h"
#include "SpriteRenderer.h"
#include "TextureCoordinates.h"
#include "Vertex3D.h"
#include "TopLevelAccelerationStructureVk.h"
#include "IGpuMemorySubblock.h"
#include "UiElement.h"
#include "Viewport.h"
#include "Lights.h"
#include "SkyboxRenderSystem.h"
#include "CollisionComponent.h"
#include "AxisAlignedBoundingBox.h"

#include "StaticGBufferPass.h"
#include "AnimatedGBufferPass.h"
#include "ShadowsStaticPass.h"

#include "Math.h"

#include "ConsoleCommandExecutor.h"

#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "DeferredRenderSystem.h"
#include "GdrDeferredSystem.h"
#include "RenderTarget.h"

#include "AudioSourceAl.h"

#include "ModelLoader3D.h"
#include "IrradianceMap.h"
#include "SpecularMap.h"

#include "PreBuiltCollider.h"
#include "PreBuiltColliderLoader.h"

#include "FxaaPass.h"
#include "ToneMapping.h"
#include "BloomPass.h"
#include "SmaaPass.h"
#include "HbaoPass.h"

#include "ColliderRenderer.h"
#include "RenderBoundsRenderer.h"
#include "SphereCollider.h"
#include "ConvexVolume.h"
#include "PhysicsComponent.h"
#include "FrameCombiner.h"

#include "IAudioApi.h"
#include "AudioAsset.h"
#include "IAudioSource.h"
#include "CollisionEvent.h"

#include "PreBuiltSpline3D.h"
#include "UiConsole.h"

#include "FileIO.h"
#include "StopWatch.h"

#include "EditorUi.h"
#include "EditorUiConstants.h"

#include "SdfBindlessRenderer2D.h"
#include "SdfStringInfo.h"

#include <thread>


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

#define OSK_CURRENT_RSYSTEM OSK::ECS::DeferredRenderSystem

class Game1 : public OSK::IDebugGame {

public:

	Game1() : OSK::IDebugGame(GAME::DefaultContentProfile::ALL) {

	}

protected:

	void CreateWindow() override {
		Engine::GetDisplay()->Create({ 1800u, 900u }, "OSKengine");

		IO::IMouseInput* mouseInput = nullptr;
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouseInput);

		if (mouseInput) {
			mouseInput->SetReturnMode(IO::MouseReturnMode::FREE);
			mouseInput->SetMotionMode(IO::MouseMotionMode::RAW);
		}
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetDisplay(), PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
		IDebugGame::OnCreate();

		StopWatch stopWatch{};
		stopWatch.Start();

		// Material load
		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/direct_pbr.json"); //Resources/PbrMaterials/deferred_gbuffer.json
		material2d = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json");
		materialRenderTarget = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_rendertarget.json");

		// SpawnCircuit();
		SpawnPista();
		SpawnBall();
		SpawnCamera();
		SpawnCamera2D();

		SetupRenderSystems();
		
		SpawnSecondCollider();
		
		SetupRenderTargets();
		SetupPostProcessingChain();

		// Esperar a la carga completa.
		Engine::GetJobSystem()->WaitForJobs<AssetLoaderJob>();

		stopWatch.Stop();

		Engine::GetLogger()->InfoLog(std::format("Tiempo de carga: {} segundos", stopWatch.GetElapsedTime()));

		m_sdfRenderer = MakeUnique<SdfBindlessRenderer2D>(
			Engine::GetEcs(),
			Engine::GetRenderer()->GetAllocator(),
			Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(SdfBindlessRenderer2D::DefaultMaterialPath));
	}

	void RegisterSystems() override {
		Engine::GetEcs()->RemoveSystem<ECS::RenderSystem2D>();

		ECS::SystemDependencies debugRenderDependencies{};
		debugRenderDependencies.executeAfterThese.insert(static_cast<std::string>(ECS::DeferredRenderSystem::GetSystemName()));
		Engine::GetEcs()->RegisterSystem<ECS::ColliderRenderSystem>(debugRenderDependencies);
		Engine::GetEcs()->RegisterSystem<ECS::RenderBoundsRenderer>(debugRenderDependencies);
	}
	
	void OnTick_BeforeEcs(TDeltaTime deltaTime) override {
		IDebugGame::OnTick_BeforeEcs(deltaTime);

		const auto _ = Engine::GetEcs();

		IO::IKeyboardInput* keyboard = nullptr;
		IO::IGamepadInput* gamepad = nullptr;
		IO::IMouseInput* mouse = nullptr;

		// Obtenemos las interfaces necesarias
		Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);
		Engine::GetInput()->QueryInterface(IUUID::IGamepadInput, (void**)&gamepad);
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouse);


		CameraComponent3D& camera = Engine::GetEcs()->GetComponent<ECS::CameraComponent3D>(cameraObject);
		auto& cameraTransform = Engine::GetEcs()->GetComponent<ECS::TransformComponent3D>(cameraObject).GetTransform();

		// Movimiento de la c�mara en este frame
		float cameraForwardMovement = 0.0f;
		float cameraRightMovement = 0.0f;
		Vector2f cameraRotation = Vector2f::Zero;

		const auto* ecs = Engine::GetEcs();
		auto* editorUi = GetRootUiElement().GetChild(Editor::UI::EditorUi::Name)->As<Editor::UI::EditorUi>();
		editorUi->Update(ecs, deltaTime);

		// Console
		if (IsInConsole()) {
			goto _cont;
		}

		// Si disponemos de teclado...
		if (keyboard) {

			// Exit
			if (keyboard->IsKeyDown(IO::Key::ESCAPE))
				this->Exit();

			// Fullscreen
			if (keyboard->IsKeyStroked(IO::Key::F11)) {
				IO::IFullscreenableDisplay* display = nullptr;
				Engine::GetDisplay()->QueryInterface(OSK_IUUID(IO::IFullscreenableDisplay), (void**)&display);

				if (display)
					display->ToggleFullscreen();
			}



#pragma region Exposici�n y gamma

			// Exposici�n (~brillo) de la escena
			const bool increaseExposure = keyboard->IsKeyDown(IO::Key::F1);
			const bool decreaseExposure = keyboard->IsKeyDown(IO::Key::F2);

			const bool exposureChanged = increaseExposure || decreaseExposure;

			if (increaseExposure)
				toneMappingPass->SetExposure(toneMappingPass->GetExposure() - deltaTime * 2);

			if (decreaseExposure)
				toneMappingPass->SetExposure(toneMappingPass->GetExposure() + deltaTime * 2);

			if (exposureChanged) {
				for (auto& buffer : exposureBuffers) {
					buffer->MapMemory();
					buffer->Write(toneMappingPass->GetExposure());
					buffer->Unmap();
				}
			}

			// Gamma de la escena
			if (keyboard->IsKeyDown(IO::Key::J))
				toneMappingPass->SetGamma(toneMappingPass->GetGamma() - deltaTime * 2);
			if (keyboard->IsKeyDown(IO::Key::K))
				toneMappingPass->SetGamma(toneMappingPass->GetGamma() + deltaTime * 2);

#pragma endregion

			// Configuraci�n gr�fica
			//
			// Bloom
			if (keyboard->IsKeyReleased(IO::Key::B)) {
				SetBloomState(!config.useBloom);
			}

			// #ifdef OSK_USE_DEFERRED_RENDERER  
						// TAA
			if (keyboard->IsKeyReleased(IO::Key::T)) {
				Engine::GetEcs()->GetSystem<DeferredRenderSystem>()->ToggleTaa();
			}

			if (keyboard->IsKeyReleased(IO::Key::H)) {
				SetHbaoState(!config.useHbao);
			}

			if (keyboard->IsKeyStroked(IO::Key::R)) {
				Engine::GetRenderer()->GetMaterialSystem()->ReloadAllMaterials();
			}

			if (keyboard->IsKeyStroked(IO::Key::M)) {
				IO::IMouseInput* mouseInput = nullptr;
				Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouseInput);

				if (mouseInput) {
					const IO::MouseReturnMode previousMode = mouseInput->GetReturnMode();
					const IO::MouseReturnMode nextMode = previousMode == IO::MouseReturnMode::ALWAYS_RETURN
						? IO::MouseReturnMode::FREE
						: IO::MouseReturnMode::ALWAYS_RETURN;

					mouseInput->SetReturnMode(nextMode);
				}
			}

			// Activar / desactivar renderizado de colliders
			if (keyboard->IsKeyReleased(IO::Key::C)){
				Engine::GetEcs()->GetSystem<ColliderRenderSystem>()->ToggleActivationStatus();
			}


			// Movimiento de la c�mara
			if (keyboard->IsKeyDown(IO::Key::W))
				cameraForwardMovement += 2.0f;
			if (keyboard->IsKeyDown(IO::Key::S))
				cameraForwardMovement -= 2.0f;
			if (keyboard->IsKeyDown(IO::Key::A))
				cameraRightMovement -= 2.0f;
			if (keyboard->IsKeyDown(IO::Key::D))
				cameraRightMovement += 2.0f;

			if (keyboard->IsKeyDown(IO::Key::LEFT_SHIFT)) {
				cameraForwardMovement *= 0.3f;
				cameraRightMovement *= 0.3f;
			}
			if (keyboard->IsKeyDown(IO::Key::LEFT_CONTROL)) {
				cameraForwardMovement *= 2.3f;
				cameraRightMovement *= 2.3f;
			}

			// Player test
			auto* playerPhysics = &Engine::GetEcs()->GetComponent<PhysicsComponent>(m_otherObject);
			if (keyboard->IsKeyDown(IO::Key::UP))
				playerPhysics->ApplyForce(Vector3f(0, 0, 1) * 500.0f * deltaTime);
			if (keyboard->IsKeyDown(IO::Key::DOWN))
				playerPhysics->ApplyForce(Vector3f(0, 0, 1) * -500.0f * deltaTime);
			if (keyboard->IsKeyDown(IO::Key::LEFT))
				playerPhysics->ApplyForce(Vector3f(1, 0, 0) * 500.0f * deltaTime);
			if (keyboard->IsKeyDown(IO::Key::RIGHT))
				playerPhysics->ApplyForce(Vector3f(1, 0, 0) * -500.0f * deltaTime);

			if (keyboard->IsKeyStroked(IO::Key::F2)) {
				Engine::GetRenderer()->_GetSwapchain()->TakeScreenshot("screenshot");
			}

			if (keyboard->IsKeyStroked(IO::Key::M)) {
				mouse->SetReturnMode(IO::MouseReturnMode::ALWAYS_RETURN);
			}

			if (keyboard->IsKeyStroked(IO::Key::SPACE)) {
				auto direccion = cameraTransform.GetForwardVector().GetNormalized();

				if (keyboard->IsKeyDown(IO::Key::LEFT_SHIFT)) {
					Engine::GetEcs()->GetComponent<PhysicsComponent>(m_ballObject).ApplyImpulse(-direccion * 2.0f, Vector3f::Zero);
				} else {
					direccion.y *= -1.0f;
					Engine::GetEcs()->GetComponent<PhysicsComponent>(m_ballObject).ApplyImpulse(direccion * 2.0f, Vector3f::Zero);
				}
			}
		}

		if (mouse) {
			cameraRotation = {
				(float)(mouse->GetMouseState().GetPosition().x - mouse->GetPreviousMouseState().GetPosition().x),
				(float)(mouse->GetMouseState().GetPosition().y - mouse->GetPreviousMouseState().GetPosition().y)
			};
		}
		
		cameraTransform.AddPosition(cameraTransform.GetForwardVector() * cameraForwardMovement * deltaTime);
		cameraTransform.AddPosition(cameraTransform.GetRightVector() * cameraRightMovement * deltaTime);
		cameraTransform.RotateWorldSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
		cameraTransform.RotateLocalSpace(glm::radians(-cameraRotation.y * 0.25f), { 1, 0, 0 });

		camera.UpdateTransform(&cameraTransform);

		//
		hbaoPass->UpdateCamera(
			glm::inverse(camera.GetProjectionMatrix()), 
			camera.GetViewMatrix(cameraTransform),
			camera.GetNearPlane());

		Engine::GetEcs()->GetComponent<CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			Engine::GetEcs()->GetComponent<Transform2D>(cameraObject2d)
		);

		if (false) {
			const float targetFps = 30.0f;
			const float targetMs = 1.0f / targetFps;
			const auto waitMiliseconds = static_cast<long>((targetMs - deltaTime) * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(glm::max(0l, waitMiliseconds)));
		}

		{
			// A
			auto& ballTransform = Engine::GetEcs()->GetComponent<TransformComponent3D>(m_ballObject).GetTransform();
			if (ballTransform.GetPosition().z > 10 || ballTransform.GetPosition().z < -10 ||
				ballTransform.GetPosition().x > 20 || ballTransform.GetPosition().x < -20) 
			{
				ballTransform.SetPosition({ 0.0f, 2.0f, 0.0f });
				Engine::GetEcs()->GetComponent<PhysicsComponent>(m_ballObject)._SetVelocity(Vector3f::Zero);
			}
		}

	_cont:

		// UI
		if (mouse) {
			const bool isPressed = mouse->GetMouseState().IsButtonDown(IO::MouseButton::BUTTON_LEFT);
			const Vector2f position = mouse->GetMouseState().GetPosition().ToVector2f();

			GetRootUiElement().UpdateByCursor(position, isPressed);
		}

		if (keyboard) {
			GetRootUiElement().UpdateByKeyboard(keyboard->GetPreviousKeyboardState(), keyboard->GetKeyboardState());
		}
	}

	void BuildFrame() override {
		auto commandList = Engine::GetRenderer()->GetMainCommandList();
		auto renderpass = Engine::GetRenderer()->GetFinalRenderTarget();

		const Vector4ui windowRec = {
			0,
			0,
			Engine::GetDisplay()->GetResolution().x,
			Engine::GetDisplay()->GetResolution().y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		commandList->SetViewport(viewport);
		commandList->SetScissor(windowRec);

		// Render text
		commandList->StartDebugSection("Text Rendering", Color::Blue);
		commandList->BeginGraphicsRenderpass(textRenderTarget.GetPointer(), Color::Black * 0.0f);

		{
			SdfDrawCall2D debug{};
			debug.transform.SetPosition(Vector2f{ 0.0f });
			debug.transform.SetScale(Vector2f{ 1920.0f, 1080.0f });
			debug.shape = SdfShape2D::RECTANGLE;
			debug.contentType = SdfDrawCallContentType2D::TEXTURE;
			GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
			viewConfig.channel = SampledChannel::COLOR;
			debug.texture = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>()->GetGbuffer().GetImage(GBuffer::Target::COLOR)->GetView(viewConfig);
			debug.mainColor = Color::White;

			m_sdfRenderer->Begin(commandList);
			m_sdfRenderer->SetCamera(cameraObject2d);
			// m_sdfRenderer->Draw(debug);

			// GetRootUiElement().Render(m_sdfRenderer.GetPointer());

			m_sdfRenderer->End();
		}

		commandList->EndGraphicsRenderpass();
		commandList->EndDebugSection();

		// Full-screen rendering
		//
		// Pre-Effects
		commandList->StartDebugSection("Pre-Effects Frame build", Color(0, 1, 0));

		const auto* skyboxRenderSystem = Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>();
		const auto* renderSystem = Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>();

		auto skyboxRenderSystemImg = Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->GetRenderTarget().GetMainColorImage();
		auto renderSystemImg = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>()->GetRenderTarget().GetColorImage(0);

		commandList->SetGpuImageBarrier(
			skyboxRenderSystemImg,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));
		commandList->SetGpuImageBarrier(
			renderSystemImg,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

		GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

		preEffectsFrameCombiner->Begin(commandList, FrameCombiner::ImageFormat::RGBA16);
		preEffectsFrameCombiner->Draw(commandList, *skyboxRenderSystemImg->GetView(viewConfig));
		preEffectsFrameCombiner->Draw(commandList, *renderSystemImg->GetView(viewConfig));
		preEffectsFrameCombiner->End(commandList);

		commandList->EndDebugSection();

		// Post-processing effects:
		if (config.useHbao)
			hbaoPass->Execute(commandList);
		if (config.useBloom)
			bloomPass->Execute(commandList);
		toneMappingPass->Execute(commandList);

		// Frame build:
		commandList->StartDebugSection("Final Frame build", Color(0, 1, 0));
		auto* colliderRenderSystem = Engine::GetEcs()->GetSystem<ECS::ColliderRenderSystem>();
		if (colliderRenderSystem) {
			commandList->SetGpuImageBarrier(
				colliderRenderSystem->GetRenderTarget().GetMainColorImage(),
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		}

		auto* boundsRenderSystem = Engine::GetEcs()->GetSystem<ECS::RenderBoundsRenderer>();
		if (boundsRenderSystem) {
			commandList->SetGpuImageBarrier(
				boundsRenderSystem->GetRenderTarget().GetMainColorImage(),
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		}

		// Sync
		commandList->SetGpuImageBarrier(
			toneMappingPass->GetOutput().GetTargetImage(),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		commandList->SetGpuImageBarrier(
			textRenderTarget->GetMainColorImage(),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));

		commandList->BindVertexBuffer(Sprite::globalVertexBuffer.GetValue());
		commandList->BindIndexBuffer(Sprite::globalIndexBuffer.GetValue());

		commandList->SetViewport(viewport);
		commandList->SetScissor(windowRec);

		commandList->BeginGraphicsRenderpass(renderpass);
		commandList->BindMaterial(*Engine::GetRenderer()->GetFullscreenRenderingMaterial());

		commandList->BindMaterialSlot(*toneMappingPass->GetOutput().GetFullscreenSpriteMaterialSlot());
		commandList->DrawSingleInstance(6);

		if (colliderRenderSystem && colliderRenderSystem->IsActive()) {
			commandList->BindMaterialSlot(*colliderRenderSystem->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
			commandList->DrawSingleInstance(6);
		}

		if (boundsRenderSystem && boundsRenderSystem->IsActive()) {
			/*frameBuildCommandList->BindMaterialSlot(*boundsRenderSystem->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
			frameBuildCommandList->DrawSingleInstance(6);*/
		}

		commandList->BindMaterialSlot(*textRenderTarget->GetFullscreenSpriteMaterialSlot());
		commandList->DrawSingleInstance(6);

		commandList->EndGraphicsRenderpass();

		commandList->EndDebugSection();

		commandList->SetGpuImageBarrier(
			Engine::GetRenderer()->_GetSwapchain()->GetImage(Engine::GetRenderer()->GetCurrentFrameIndex()),
			GpuImageLayout::COLOR_ATTACHMENT,
			GpuImageLayout::PRESENT,
			GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
			GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));
	}


	void OnWindowResize(const Vector2ui& size) override {
		textRenderTarget->Resize(size);
		preEffectsRenderTarget->Resize(size);
		bloomPass->Resize(size);
		hbaoPass->Resize(size);
		toneMappingPass->Resize(size);

		preEffectsFrameCombiner->Resize(size);

		SetupPostProcessingChain();
	}

	void OnExit() override {
		m_sdfRenderer.Delete();

		bloomPass.Delete();
		hbaoPass.Delete();
		toneMappingPass.Delete();

		for (auto& buffer : exposureBuffers)
			buffer.Delete();

		preEffectsRenderTarget.Delete();
		textRenderTarget.Delete();

		preEffectsFrameCombiner.Delete();
	}

private:

	void SetTaaState(bool) {
		auto renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		renderSystem->ToggleTaa();
	}

	void SetHbaoState(bool state) {
		config.useHbao = state;
		SetupPostProcessingChain();
	}

	void SetBloomState(bool state) {
		config.useBloom = state;
		SetupPostProcessingChain();
	}

	void SetupRenderTargets() {
		// Text
		RenderTargetAttachmentInfo textColorInfo{ .format = Format::RGBA8_SRGB, .usage = GpuImageUsage::SAMPLED, .name = "Text Color Target" };
		RenderTargetAttachmentInfo textDepthInfo{ .format = Format::D16_UNORM , .name = "Text Depth Target" };
		textRenderTarget = MakeUnique<RenderTarget>();
		textRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { textColorInfo }, textDepthInfo);

		// Pre effects scene
		RenderTargetAttachmentInfo preEffectsColorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE, .name = "Pre-Effects Color Target" };
		RenderTargetAttachmentInfo preEffectsDepthInfo{ .format = Format::D16_UNORM, .name = "Pre-Effects Depth Target" };
		preEffectsRenderTarget = MakeUnique<RenderTarget>();
		preEffectsRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { preEffectsColorInfo }, preEffectsDepthInfo);

		// Combiners
		preEffectsFrameCombiner = MakeUnique<FrameCombiner>();
		preEffectsFrameCombiner->Create(Engine::GetDisplay()->GetResolution(), preEffectsColorInfo);
	}

	void SetupPostProcessingChain() {
		// Inicializaciones
		if (!hbaoPass.HasValue()) {
			hbaoPass = MakeUnique<HbaoPass>();
			hbaoPass->Create(Engine::GetDisplay()->GetResolution());
		}

		if (!bloomPass.HasValue()) {
			bloomPass = MakeUnique<BloomPass>();
			bloomPass->Create(Engine::GetDisplay()->GetResolution());
		}

		if (!toneMappingPass.HasValue()) {
			toneMappingPass = MakeUnique<ToneMappingPass>();
			toneMappingPass->Create(Engine::GetDisplay()->GetResolution());

			const GpuBuffer* epxBuffers[MAX_RESOURCES_IN_FLIGHT]{};
			for (UIndex32 i = 0; i < exposureBuffers.size(); i++) {
				exposureBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(float), GpuQueueType::MAIN);
				epxBuffers[i] = exposureBuffers[i].GetPointer();

				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}

			toneMappingPass->SetExposureBuffer(*exposureBuffers[0]);
		}

		const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

		auto& preEffectsSource = preEffectsFrameCombiner->GetRenderTarget();

		auto* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();

		if (config.useHbao) {
			hbaoPass->SetColorInput(preEffectsSource.GetTargetImage());
			hbaoPass->SetNormalsInput(renderSystem->GetGbuffer().GetImage(GBuffer::Target::NORMAL));
			hbaoPass->SetDepthInput(renderSystem->GetGbuffer().GetImage(GBuffer::Target::DEPTH));
		}

		if (config.useBloom && config.useHbao) {
			bloomPass->SetInput(hbaoPass->GetOutput().GetTargetImage());
			toneMappingPass->SetInput(bloomPass->GetOutput().GetTargetImage());
		} else
		if (config.useBloom) {
			bloomPass->SetInput(preEffectsSource.GetTargetImage());
			toneMappingPass->SetInput(bloomPass->GetOutput().GetTargetImage());
		}
		else 
		if (config.useHbao) {
			toneMappingPass->SetInput(hbaoPass->GetOutput().GetTargetImage());
		}
		else {
			toneMappingPass->SetInput(preEffectsSource.GetTargetImage());
		}

		Engine::GetRenderer()->WaitForCompletion();
	}

	void SetupRenderSystems() {
		DynamicArray<std::string> tags{};
		tags.Insert("RENDER_SYSTEM_RESOURCES");

		auto skyboxTexture = Engine::GetAssetManager()->LoadAsync<CubemapTexture>("Resources/Assets/Skyboxes/skybox0.json", tags.GetFullSpan());
		auto specularMap = Engine::GetAssetManager()->LoadAsync<SpecularMap>("Resources/Assets/IBL/irradiance0.json", tags.GetFullSpan());
		auto irradianceMap = Engine::GetAssetManager()->LoadAsync<IrradianceMap>("Resources/Assets/IBL/specular0.json", tags.GetFullSpan());

		Engine::GetJobSystem()->WaitForTag(tags[0]);

		// PBR Render System
		OSK_CURRENT_RSYSTEM* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		renderSystem->Initialize(cameraObject, irradianceMap, specularMap);

#ifdef OSK_USE_GDR_RENDERER
		renderSystem->SetMaxCounts(4'000'000, 100'000);
#endif // OSK_USE_GDR_RENDERER

		// Skybox Render System
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCamera(cameraObject);
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCubemap(skyboxTexture);

		// Debug Render Systems
		Engine::GetEcs()->GetSystem<ColliderRenderSystem>()->Initialize(cameraObject);
		Engine::GetEcs()->GetSystem<RenderBoundsRenderer>()->Initialize(cameraObject);

	}

	void SpawnCamera() {
		cameraObject = Engine::GetEcs()->SpawnObject();
		auto* cameraTransform = &Engine::GetEcs()->AddComponent<TransformComponent3D>(cameraObject, TransformComponent3D(cameraObject)).GetTransform();
		cameraTransform->AddPosition({ 0.0f, 0.1f, -1.1f });
		Engine::GetEcs()->AddComponent<CameraComponent3D>(cameraObject, {});
	}

	void SpawnCamera2D() {
		cameraObject2d = Engine::GetEcs()->SpawnObject();
		CameraComponent2D* camera2D = &Engine::GetEcs()->AddComponent<CameraComponent2D>(cameraObject2d, {});
		camera2D->LinkToDisplay(Engine::GetDisplay());
		Engine::GetEcs()->AddComponent<Transform2D>(cameraObject2d, Transform2D(cameraObject2d));
	}

	void SpawnCircuit() {
		return;
		auto circuitObject = Engine::GetEcs()->SpawnObject();

		// Transform
		Engine::GetEcs()->AddComponent<TransformComponent3D>(circuitObject, TransformComponent3D(circuitObject));

		// Modelo 3D
		auto circuitModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/circuit0.json");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(circuitObject, {});

		modelComponent->SetModel(circuitModel); // animModel
		modelComponent->AddShaderPassName(StaticGBufferPass::Name);
		modelComponent->AddShaderPassName(ShadowsStaticPass::Name);


#define LOADED_COLLIDER
#ifdef LOADED_COLLIDER
		CollisionComponent colliderComponent{};
		colliderComponent.SetCollider(Engine::GetAssetManager()->Load<PreBuiltCollider>("Resources/Assets/Collision/circuit_colliders.json"));
#else
		Collider collider = {};
		OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 100.0f, 100.0f, 100.0f }, -100.0f));

		collider.SetTopLevelCollider(new AxisAlignedBoundingBox({ 100.0f, 5.0f, 100.0f }));
		collider.AddBottomLevelCollider(convexVolume.GetPointer());
#endif // LOADED_COLLIDER

		Engine::GetEcs()->AddComponent<CollisionComponent>(circuitObject, std::move(colliderComponent));

		PhysicsComponent physicsComponent{};
		physicsComponent.SetImmovable();
		physicsComponent.coefficientOfRestitution = 0.8f;
		physicsComponent.frictionCoefficient = 1.0f;
		Engine::GetEcs()->AddComponent<PhysicsComponent>(circuitObject, std::move(physicsComponent));

		// Billboards
		const auto billboards = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<TransformComponent3D>(billboards, TransformComponent3D(billboards));

		auto billboardsModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/circuit0_billboards.json");

		ModelComponent3D* billboardsModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(billboards, {});

		billboardsModelComponent->SetModel(billboardsModel); // animModel
		PhysicsComponent billboardsPhysicsComponent{};
		billboardsPhysicsComponent.SetImmovable();
		physicsComponent.coefficientOfRestitution = 0.8f;
		Engine::GetEcs()->AddComponent<PhysicsComponent>(billboards, std::move(billboardsPhysicsComponent));

		// Tree normals
		const auto treeNormals = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<TransformComponent3D>(treeNormals, TransformComponent3D(treeNormals));

		auto treeNormalsModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/circuit0_tree_normals.json");

		ModelComponent3D* treeNormalsModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(treeNormals, {});

		treeNormalsModelComponent->SetModel(treeNormalsModel); // animModel
		treeNormalsModelComponent->SetCastShadows(false);

		PhysicsComponent treeNormalsPhysicsComponent{};
		treeNormalsPhysicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(treeNormals, std::move(treeNormalsPhysicsComponent));

		// Trees
		const auto trees = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<TransformComponent3D>(trees, TransformComponent3D(trees));

		auto treesModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/circuit0_trees.json");

		ModelComponent3D* treesModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(trees, {});

		treesModelComponent->SetModel(treesModel); // animModel
		treesModelComponent->SetCastShadows(false);

		PhysicsComponent treesPhysicsComponent{};
		treesPhysicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(trees, std::move(treesPhysicsComponent));

		// Spline test
		auto spline = Engine::GetAssetManager()->LoadAsync<PreBuiltSpline3D>("Resources/Assets/Curves/circuit0.json");
		auto& loadedSpline = spline->Get();
	}

	void SpawnSecondCollider() {
		m_otherObject = Engine::GetEcs()->SpawnObject();

		// Transform
		auto* transform = &Engine::GetEcs()->AddComponent<TransformComponent3D>(m_otherObject, TransformComponent3D(m_otherObject)).GetTransform();
		transform->AddPosition({ 0.0f, 0.0f, 0.0f });

		// Setup de f�sicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(m_otherObject, {});
		physicsComponent.SetMass(2.0f);
		physicsComponent.centerOfMassOffset = Vector3f::Zero;

		// Collider
		CollisionComponent collider{};
		collider.SetCollider(Collider());

		UniquePtr<ConvexVolume> convexVolume = MakeUnique<ConvexVolume>(ConvexVolume::CreateObb({ 0.2f * 2, 0.52f * 2, 0.2f * 2 }));

		collider.GetCollider()->SetTopLevelCollider(AsBroad(MakeUnique<AxisAlignedBoundingBox>(Vector3f(20.0f))));
		collider.GetCollider()->AddBottomLevelCollider(AsNarrow(std::move(convexVolume)));

		Engine::GetEcs()->AddComponent<CollisionComponent>(m_otherObject, std::move(collider));

		// Render
		auto model = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/player.json");
		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(m_otherObject, {});
		modelComponent->SetModel(model);
		modelComponent->AddShaderPassName(AnimatedGBufferPass::Name);
		modelComponent->GetModel()->GetAnimator().AddActiveAnimation("ArmatureAction");
		modelComponent->GetModel()->GetAnimator().SetActiveSkin("Armature");
		// modelComponent->AddShaderPassName(ShadowsStaticPass::Name);
	}

	void SpawnBall() {
		m_ballObject = Engine::GetEcs()->SpawnObject();

		// Transform
		auto* transform = &Engine::GetEcs()->AddComponent<TransformComponent3D>(m_ballObject, TransformComponent3D(m_ballObject)).GetTransform();
		transform->AddPosition({ 2.0f, 3.0f, 0.0f });

		// Setup de f�sicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(m_ballObject, {});
		physicsComponent.SetMass(0.5f);
		physicsComponent.centerOfMassOffset = Vector3f::Zero;
		physicsComponent.coefficientOfRestitution = 0.8f;
		physicsComponent.frictionCoefficient = 1.2f;
		physicsComponent.SetInverseInertiaTensor(glm::mat3(1.0f) * 200.0f);

		// Collider
		auto sphere = MakeUnique<SphereCollider>(0.1f);
		CollisionComponent collider{};
		collider.SetCollider(Collider());
		collider.GetCollider()->SetTopLevelCollider(AsBroad(sphere->CreateBroadCopy()));
		collider.GetCollider()->AddBottomLevelCollider(AsNarrow(std::move(sphere)));
		Engine::GetEcs()->AddComponent<CollisionComponent>(m_ballObject, std::move(collider));

		// Render
		auto ballModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/sphere.json");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(m_ballObject, {});

		modelComponent->SetModel(ballModel); // animModel
		modelComponent->AddShaderPassName(StaticGBufferPass::Name);
		modelComponent->AddShaderPassName(ShadowsStaticPass::Name);
	}

	void SpawnPista() {
		auto pista = Engine::GetEcs()->SpawnObject();

		// Transform
		auto* transform = &Engine::GetEcs()->AddComponent<TransformComponent3D>(pista, TransformComponent3D(pista)).GetTransform();

		// Setup de f�sicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(pista, {});
		physicsComponent.coefficientOfRestitution = 0.3f;
		physicsComponent.frictionCoefficient = 0.8f;
		physicsComponent.SetImmovable();

		// Collider
		CollisionComponent colliderComponent{};
		Collider collider = {};
		UniquePtr<ConvexVolume> convexVolume = MakeUnique<ConvexVolume>(ConvexVolume::CreateObb({ 100.0f, 100.0f, 100.0f }));
		convexVolume->AddOffset({ 0.0f, -100.0f, 0.0f });
		collider.SetTopLevelCollider(AsBroad(MakeUnique<AxisAlignedBoundingBox>(Vector3f{ 100.0f, 5.0f, 100.0f })));
		collider.AddBottomLevelCollider(AsNarrow(std::move(convexVolume)));
		colliderComponent.SetCollider(collider);

		Engine::GetEcs()->AddComponent<CollisionComponent>(pista, std::move(colliderComponent));

		// Render
		auto ballModel = Engine::GetAssetManager()->LoadAsync<Model3D>("Resources/Assets/Models/pista.json");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(pista, {});

		modelComponent->SetModel(ballModel); // animModel
		modelComponent->AddShaderPassName(StaticGBufferPass::Name);
		modelComponent->AddShaderPassName(ShadowsStaticPass::Name);
	}

	struct CollisionTestCase {
		std::string name = "";
		Vector3f positionA = Vector3f::Zero;
		Vector3f positionB = Vector3f::Zero;
	};

	const DynamicArray<CollisionTestCase> testCases = {
		{ "T-COL-00", {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{ "T-COL-01", {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, -1.0f} },
		{ "T-COL-02", {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
		{ "T-COL-03", {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-04", {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
		{ "T-COL-05", {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-06", {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
		{ "T-COL-07", {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, -1.0f} },

		{ "T-COL-08", {0.0f, 0.0f, -1.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-09", {1.0f, 0.0f, -1.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-10", {1.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-11", {1.0f, 0.0f, 1.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-12", {0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-13", {-1.0f, 0.0f, 1.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-14", {-1.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} },
		{ "T-COL-15", {-1.0f, 0.0f, -1.0f},{0.0f, 0.0f, 0.0f} },

		// +/+

		{ "T-COL-16", {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{ "T-COL-17", {1.0f, 0.0f, 1.0f}, {2.0f, 0.0f, 0.0f} },
		{ "T-COL-18", {1.0f, 0.0f, 1.0f}, {2.0f, 0.0f, 1.0f} },
		{ "T-COL-19", {1.0f, 0.0f, 1.0f}, {2.0f, 0.0f, 2.0f} },
		{ "T-COL-20", {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 2.0f} },
		{ "T-COL-21", {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f} },
		{ "T-COL-22", {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
		{ "T-COL-23", {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} },

		{ "T-COL-24", {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-25", {2.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-26", {2.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-27", {2.0f, 0.0f, 2.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-28", {1.0f, 0.0f, 2.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-29", {0.0f, 0.0f, 2.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-30", {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} },
		{ "T-COL-31", {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f} },

		// +/-

		{ "T-COL-32", {1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -2.0f}},
		{ "T-COL-33", {1.0f, 0.0f, -1.0f}, {2.0f, 0.0f, -2.0f} },
		{ "T-COL-34", {1.0f, 0.0f, -1.0f}, {2.0f, 0.0f, -1.0f} },
		{ "T-COL-35", {1.0f, 0.0f, -1.0f}, {2.0f, 0.0f, 0.0f} },
		{ "T-COL-36", {1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
		{ "T-COL-37", {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f} },
		{ "T-COL-38", {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
		{ "T-COL-39", {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -2.0f} },

		{ "T-COL-40", {1.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -1.0f} },
		{ "T-COL-41", {2.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -1.0f} },
		{ "T-COL-42", {2.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f} },
		{ "T-COL-43", {2.0f, 0.0f, 0.0f} , {1.0f, 0.0f, -1.0f} },
		{ "T-COL-44", {1.0f, 0.0f, 0.0f} , {1.0f, 0.0f, -1.0f} },
		{ "T-COL-45", {0.0f, 0.0f, 0.0f} , {1.0f, 0.0f, -1.0f} },
		{ "T-COL-46", {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f} },
		{ "T-COL-47", {0.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -1.0f} },

		// -/-

		{ "T-COL-48", {-1.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, -2.0f}},
		{ "T-COL-49", {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -2.0f} },
		{ "T-COL-50", {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
		{ "T-COL-51", {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f} },
		{ "T-COL-52", {-1.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ "T-COL-53", {-1.0f, 0.0f, -1.0f}, {-2.0f, 0.0f, 0.0f} },
		{ "T-COL-54", {-1.0f, 0.0f, -1.0f}, {-2.0f, 0.0f, -1.0f} },
		{ "T-COL-55", {-1.0f, 0.0f, -1.0f}, {-2.0f, 0.0f, -2.0f} },

		{ "T-COL-56", {-1.0f, 0.0f, -2.0f}, {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-57", {0.0f, 0.0f, -2.0f}, {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-58", {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-59", {0.0f, 0.0f, 0.0f} , {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-60", {-1.0f, 0.0f, 0.0f} , {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-61", {-2.0f, 0.0f, 0.0f} , {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-62", {-2.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, -1.0f} },
		{ "T-COL-63", {-2.0f, 0.0f, -2.0f}, {-1.0f, 0.0f, -1.0f} },

		// -/+

		{ "T-COL-64", {-1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{ "T-COL-65", {-1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f} },
		{ "T-COL-66", {-1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f} },
		{ "T-COL-67", {-1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 2.0f} },
		{ "T-COL-68", {-1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 2.0f} },
		{ "T-COL-69", {-1.0f, 0.0f, 1.0f}, {-2.0f, 0.0f, 2.0f} },
		{ "T-COL-70", {-1.0f, 0.0f, 1.0f}, {-2.0f, 0.0f, 1.0f} },
		{ "T-COL-71", {-1.0f, 0.0f, 1.0f}, {-2.0f, 0.0f, 0.0f} },

		{ "T-COL-72", {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-73", { 0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-74", { 0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-75", { 0.0f, 0.0f, 2.0f} , {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-76", {-1.0f, 0.0f, 2.0f} , {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-77", {-2.0f, 0.0f, 2.0f} , {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-78", {-2.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 1.0f} },
		{ "T-COL-79", {-2.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 1.0f} },

	};

	struct Config {
		bool useHbao = true;
		bool useBloom = true;
	} config;

	UniquePtr<FrameCombiner> preEffectsFrameCombiner;

	UniquePtr<RenderTarget> preEffectsRenderTarget;
	UniquePtr<RenderTarget> textRenderTarget;

	UniquePtr<BloomPass> bloomPass;
	UniquePtr<ToneMappingPass> toneMappingPass;
	UniquePtr<HbaoPass> hbaoPass;

	OSK::GRAPHICS::Material* material = nullptr;
	OSK::GRAPHICS::Material* material2d = nullptr;
	OSK::GRAPHICS::Material* materialRenderTarget = nullptr;

	std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> exposureBuffers{};

	bool m_inEditor = false;

	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex m_ballObject = ECS::GameObjectIndex::CreateEmpty();
	ECS::GameObjectIndex m_otherObject = ECS::GameObjectIndex::CreateEmpty();

	UniquePtr<SdfBindlessRenderer2D> m_sdfRenderer;

};

#endif
