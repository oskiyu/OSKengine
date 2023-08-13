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
#include "GpuBuffer.h"
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

#include "UiImageView.h"
#include "UiTextView.h"
#include "UiFreeContainer.h"
#include "UiHorizontalContainer.h"
#include "UiVerticalContainer.h"
#include "UiButton.h"

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
#include "TerrainComponent.h"
#include "TerrainRenderSystem.h"
#include "TopLevelAccelerationStructureVk.h"
#include "IGpuMemorySubblock.h"
#include "UiElement.h"
#include "Viewport.h"
#include "Lights.h"
#include "SkyboxRenderSystem.h"
#include "Collider.h"
#include "AxisAlignedBoundingBox.h"

#include "Math.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "RenderTarget.h"

#include "PbrDeferredRenderSystem.h"
#include "ModelLoader3D.h"
#include "IrradianceMap.h"
#include "SpecularMap.h"

#include "FxaaPass.h"
#include "ToneMapping.h"
#include "BloomPass.h"
#include "SmaaPass.h"

#include "ColliderRenderer.h"
#include "SphereCollider.h"
#include "ConvexVolume.h"
#include "PhysicsComponent.h"
#include "FrameCombiner.h"

#include "AudioApi.h"
#include "AudioAsset.h"
#include "AudioSource.h"

OSK::GRAPHICS::SpriteRenderer spriteRenderer;


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

#if defined(OSK_USE_FORWARD_RENDERER)
#define OSK_CURRENT_RSYSTEM OSK::ECS::RenderSystem3D
#elif defined(OSK_USE_DEFERRED_RENDERER)
#define OSK_CURRENT_RSYSTEM OSK::ECS::PbrDeferredRenderSystem
#elif defined(OSK_USE_HYBRID_RENDERER)
#define OSK_CURRENT_RSYSTEM OSK::ECS::HybridRenderSystem
#endif

class Game1 : public OSK::IGame {

protected:

	void CreateWindow() override {
		Engine::GetDisplay()->Create({ 1280u, 720u }, "OSKengine");

		IO::IMouseInput* mouseInput = nullptr;
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouseInput);

		if (mouseInput) {
			mouseInput->SetReturnMode(IO::MouseReturnMode::ALWAYS_RETURN);
			mouseInput->SetMotionMode(IO::MouseMotionMode::RAW);
		}
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetDisplay(), PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
		auto animModel = Engine::GetAssetManager()->Load<ASSETS::Model3D>("Resources/Assets/Models/animmodel.json", "GLOBAL");

		// Material load
		material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/direct_pbr.json"); //Resources/PbrMaterials/deferred_gbuffer.json
		material2d = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json");
		materialRenderTarget = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_rendertarget.json");

		SpawnCar();

		SpawnCamera();
		SpawnCamera2D();

		SetupRenderSystems();
				
		SpawnCircuit();
		SpawnSecondCollider();
		
		SetupRenderTargets();
		SetupPostProcessingChain();

		spriteRenderer.SetCommandList(Engine::GetRenderer()->GetGraphicsCommandList());

		// Font loading
		font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/Fonts/font0.json", "GLOBAL");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		Engine::GetConsole()->SetFont(font);

		SetupUi();

		// Audio test
		const auto sound = Engine::GetAssetManager()->Load<ASSETS::AudioAsset>("Resources/Assets/Audio/bounce_audio.json", "GLOBAL");
		source = AUDIO::Source();
		source.Init();
		source.SetBuffer(sound->GetBuffer());
		source.SetLooping(true);
		source.SetGain(0.05f);
		source.Play();
	}

	void SetupUi() {
		auto logoContainer = new UI::HorizontalContainer({ 380.0f, 80.0f });

		const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json", "GLOBAL")
			->GetTextureView2D();
		const auto iconView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json", "GLOBAL")
			->GetTextureView2D();

		UI::ImageView* uiIcon = new UI::ImageView(Vector2f(48.0f));
		uiIcon->GetSprite().SetImageView(iconView);
		uiIcon->SetMargin(Vector4f(4.0f));

		UI::TextView* uiText = new UI::TextView(Vector2f(128.0f));
		uiText->SetFont(font);
		uiText->SetFontSize(35);
		uiText->SetText("OSKengine");
		uiText->SetAnchor(UI::Anchor::CENTER_X | UI::Anchor::CENTER_Y);
		uiText->AdjustSizeToText();

		UI::TextView* text2 = new UI::TextView(Vector2f(128.0f));
		text2->SetFont(font);
		text2->SetFontSize(21);
		text2->SetText(std::format("build {}", Engine::GetBuild()));
		text2->SetAnchor(UI::Anchor::CENTER_X | UI::Anchor::BOTTOM);
		text2->AdjustSizeToText();

		uiFpsText = new UI::TextView(Vector2f(50.0f));
		uiFpsText->SetFont(font);
		uiFpsText->SetFontSize(23);
		uiFpsText->SetText("FPS: 000");
		uiFpsText->AdjustSizeToText();
		uiFpsText->SetMargin({ 30.0f, 2.0f, 2.0f, 2.0f });

		logoContainer->GetSprite().SetImageView(uiView);
		logoContainer->GetSprite().color = Color(0.3f, 0.3f, 0.3f, 0.94f);

		logoContainer->SetPadding(Vector4f(4.0f));
		logoContainer->AddChild("icon", uiIcon);
		logoContainer->AddChild("text", uiText);
		logoContainer->AddChild("text2", text2);
		logoContainer->AddChild("fps", uiFpsText);
		// logoContainer->AddChild("image", uiImageView);

		logoContainer->SetAnchor(UI::Anchor::TOP | UI::Anchor::LEFT);

		logoContainer->AdjustSizeToChildren();
		logoContainer->Rebuild();

		GetRootUiElement().AddChild("", logoContainer);

		// Panel derecho
		UI::VerticalContainer* rightPanel = new UI::VerticalContainer(Vector2f(200.0f));

		rightPanel->GetSprite().SetImageView(uiView);
		rightPanel->GetSprite().color = Color(0.3f, 0.3f, 0.3f, 0.94f);
		rightPanel->SetAnchor(UI::Anchor::RIGHT | UI::Anchor::CENTER_Y);
		rightPanel->SetPadding(Vector2f(6.0f));

		UI::TextView* rightPanelTitle = new UI::TextView(Vector2f(50.0f));
		rightPanelTitle->SetFont(font);
		rightPanelTitle->SetFontSize(27);
		rightPanelTitle->SetText("Settings");
		rightPanelTitle->AdjustSizeToText();
		rightPanelTitle->SetMargin(Vector4f(3.0f, 3.0f, 3.0f, 6.0f));

		rightPanel->AddChild("title", rightPanelTitle);

		taaCheckbox = CreateCheckbox("TAA", [this](bool isActive) { SetTaaState(isActive); }).GetPointer();
		fxaaCheckbox = CreateCheckbox("FXAA", [this](bool isActive) { SetFxaaState(isActive); }).GetPointer();
		bloomCheckbox = CreateCheckbox("Boom", [this](bool isActive) { SetBloomState(isActive); }).GetPointer();
		collisionCheckbox = CreateCheckbox("Show Collisions", [](bool) { Engine::GetEcs()->GetSystem<ECS::ColliderRenderSystem>()->ToggleActivationStatus(); }).GetPointer();

		rightPanel->AddChild("taaCheckbox", taaCheckbox);
		rightPanel->AddChild("fxaaCheckbox", fxaaCheckbox);
		rightPanel->AddChild("bloomCheckbox", bloomCheckbox);
		rightPanel->AddChild("collisionCheckbox", collisionCheckbox);

		rightPanel->AdjustSizeToChildren();
		rightPanel->Rebuild();

		GetRootUiElement().AddChild("rightPanel", rightPanel);
	}

	void RegisterSystems() override {
		Engine::GetEcs()->RemoveSystem<ECS::RenderSystem2D>();

		Engine::GetEcs()->RegisterSystem<ECS::ColliderRenderSystem>(ECS::ISystem::DEFAULT_EXECUTION_ORDER);
	}
	
	void OnTick(TDeltaTime deltaTime) override {
		const auto _ = Engine::GetEcs();

		IO::IKeyboardInput* keyboard = nullptr;
		IO::IGamepadInput* gamepad = nullptr;
		IO::IMouseInput* mouse = nullptr;

		// Obtenemos las interfaces necesarias
		Engine::GetInput()->QueryInterface(IUUID::IKeyboardInput, (void**)&keyboard);
		Engine::GetInput()->QueryInterface(IUUID::IGamepadInput, (void**)&gamepad);
		Engine::GetInput()->QueryInterface(IUUID::IMouseInput, (void**)&mouse);


		CameraComponent3D& camera = Engine::GetEcs()->GetComponent<ECS::CameraComponent3D>(cameraObject);
		Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<ECS::Transform3D>(cameraObject);
		Transform3D& cameraArmTransform = Engine::GetEcs()->GetComponent<ECS::Transform3D>(cameraArmObject);

		// Movimiento de la cámara en este frame
		float cameraForwardMovement = 0.0f;
		float cameraRightMovement = 0.0f;
		Vector2f cameraRotation = Vector2f::Zero;


		// Si disponemos de teclado...
		if (keyboard) {

			// Exit
			if (keyboard->IsKeyDown(IO::Key::ESCAPE))
				this->Exit();
			if (keyboard->IsKeyDown(IO::Key::P))
				this->Exit();

			// Fullscreen
			if (keyboard->IsKeyStroked(IO::Key::F11)) {
				IO::IFullscreenableDisplay* display = nullptr;
				Engine::GetDisplay()->QueryInterface(OSK_IUUID(IO::IFullscreenableDisplay), (void**)&display);

				if (display)
					display->ToggleFullscreen();
			}

#pragma region Exposición y gamma

			// Exposición (~brillo) de la escena
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

			// Configuración gráfica
			//
			// Bloom
			if (keyboard->IsKeyReleased(IO::Key::B)) {
				bloomCheckbox->GetChild("button")->As<UI::Button>()->Click();
				Engine::GetConsole()->WriteLine("Bloom");
			}

			// FXAA
			if (keyboard->IsKeyReleased(IO::Key::F))
				fxaaCheckbox->GetChild("button")->As<UI::Button>()->Click();

// #ifdef OSK_USE_DEFERRED_RENDERER  
			// TAA
			if (keyboard->IsKeyReleased(IO::Key::T))
				taaCheckbox->GetChild("button")->As<UI::Button>()->Click();
		
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
			if (keyboard->IsKeyReleased(IO::Key::C))
				collisionCheckbox->GetChild("button")->As<UI::Button>()->Click();

			// Recarga de shaders
			if (keyboard->IsKeyReleased(IO::Key::R))
				Engine::GetRenderer()->GetMaterialSystem()->ReloadAllMaterials();


			// Movimiento de la cámara
			if (keyboard->IsKeyDown(IO::Key::W))
				cameraForwardMovement += 0.7f;
			if (keyboard->IsKeyDown(IO::Key::S))
				cameraForwardMovement -= 0.7f;
			if (keyboard->IsKeyDown(IO::Key::A))
				cameraRightMovement -= 0.7f;
			if (keyboard->IsKeyDown(IO::Key::D))
				cameraRightMovement += 0.7f;

			if (keyboard->IsKeyDown(IO::Key::LEFT_SHIFT)) {
				cameraForwardMovement *= 0.3f;
				cameraRightMovement *= 0.3f;
			}
		}

		if (mouse) {
			cameraRotation = {
				(float)(mouse->GetMouseState().GetPosition().x - mouse->GetPreviousMouseState().GetPosition().x),
				(float)(mouse->GetMouseState().GetPosition().y - mouse->GetPreviousMouseState().GetPosition().y)
			};
		}
			
		// TESTING
		if (keyboard->IsKeyStroked(IO::Key::Q)) {
			auto& carTransform = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
			auto& carPhysics = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);

			carTransform.SetPosition({ +0.6f, 0.6f, -3.0f });
			carTransform.SetRotation({});

			carPhysics._SetVelocity(Vector3f::Zero);
			carPhysics._SetAcceleration(Vector3f::Zero);
		}
		if (keyboard->IsKeyStroked(IO::Key::KEYPAD_0)) {
			auto& carTransform = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
			auto& carPhysics = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);

			carTransform.SetPosition(Vector3f::Zero);
			carTransform.SetRotation({});

			carPhysics._SetVelocity(Vector3f::Zero);
			carPhysics._SetAcceleration(Vector3f::Zero);
			carPhysics._SetAngularVelocity(Vector3f::Zero);

			carPhysics.ApplyImpulse(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f));
		}


		// Movimiento del coche

		Transform3D& carTransform = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
				

		PhysicsComponent& carPhysics = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);

		/**/const float carVelocityY = carPhysics.GetVelocity().y;
		const float projection = carPhysics.GetVelocity().Dot(carTransform.GetForwardVector());
		Vector3f finalVelocity = carTransform.GetForwardVector().GetNormalized() * projection;
		finalVelocity.y = carVelocityY;
		carPhysics._SetVelocity(finalVelocity);

		source.SetPitch(carPhysics.GetVelocity().GetLenght());


		constexpr static float ANGLE_PER_KEY = 45.0f;
		constexpr static float L = 1.5f;
		float currentAngle = 0.0f;

		if (keyboard) {
			
			// Rotación
			if (keyboard->IsKeyDown(IO::Key::LEFT)) {
				currentAngle += ANGLE_PER_KEY;
				// carTransform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
				if (cameraAttachedToCar) cameraRotation.x += deltaTime * 250;
			}
			if (keyboard->IsKeyDown(IO::Key::RIGHT)) {
				currentAngle -= ANGLE_PER_KEY;
				// carTransform.RotateWorldSpace(deltaTime * 2, { 0, -1, 0 });
				if (cameraAttachedToCar) cameraRotation.x -= deltaTime * 250;
			}

			if (keyboard->IsKeyDown(IO::Key::UP))
				carPhysics.ApplyForce(carTransform.GetForwardVector() * 10);
			if (keyboard->IsKeyDown(IO::Key::DOWN))
				carPhysics.ApplyForce(carTransform.GetForwardVector() * -(2.0f + 4));

			if (keyboard->IsKeyDown(IO::Key::SPACE)) {
				carPhysics._SetVelocity(Vector3f::Zero);
				carPhysics._SetAcceleration(Vector3f::Zero);
				carPhysics._SetAngularVelocity(Vector3f::Zero);
			}
		}

		if (gamepad) {
			const IO::GamepadState& gamepadState = gamepad->GetGamepadState(0);
			if (gamepadState.IsConnected()) {
				// cameraForwardMovement -= gamepadState.GetAxisState(IO::GamepadAxis::LEFT_Y);

				// cameraRightMovement += gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X);

				cameraRotation.x += gamepadState.GetAxisState(IO::GamepadAxis::RIGHT_X);
				cameraRotation.y += gamepadState.GetAxisState(IO::GamepadAxis::RIGHT_Y);

				// carPhysics.acceleration += carTransform.GetForwardVector() * 35.f * deltaTime
					// * gamepadState.GetAxisState(IO::GamepadAxis::R2);
				// carPhysics.acceleration += carTransform.GetForwardVector() * -(2.0f + 85.0f) * deltaTime
					// * gamepadState.GetAxisState(IO::GamepadAxis::L2);

				if (gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X) > 0.1f) {
					carTransform.RotateWorldSpace(deltaTime * 2 * gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X), { 0, 1.0f, 0 });
					cameraRotation.x += deltaTime * 250 * gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X);
				}
				if (gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X) < -0.1f) {
					carTransform.RotateWorldSpace(deltaTime * 2 * gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X), { 0, 1.0f, 0 });
					cameraRotation.x += deltaTime * 250 * gamepadState.GetAxisState(IO::GamepadAxis::LEFT_X);
				}
			}
		}


		// Aplicación de movimiento y rotación de la cámara
		// camera.Rotate(-cameraRotation.X, cameraRotation.Y);

		if (glm::abs(currentAngle) > 0.01f && Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetVelocity().GetLenght() > 1.0f) {
			// Slip angles
			const Vector3f front = Engine::GetEcs()->GetComponent<ECS::Transform3D>(carObject).GetForwardVector();
			const Vector3f right = Engine::GetEcs()->GetComponent<ECS::Transform3D>(carObject).GetForwardVector();
			const Vector3f v = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetVelocity();

			const float v_lat = v.Dot(right);
			const float v_long = v.Dot(front);

			const float angle_front = glm::atan(v_lat / v_long) - glm::radians(currentAngle) * glm::sign(v_long);
			const float angle_rear = glm::atan(v_lat / v_long);

			const float C_a = -3.0f;
			const float f_lat_front = C_a * angle_front;
			const float f_lat_rear = C_a * angle_rear;
			const float torque = f_lat_front * 0.75f - f_lat_rear * 0.75f; // glm::cos(glm::radians(currentAngle)) * 

			const float R = L / glm::sin(glm::radians(currentAngle));
			const float w = v.GetLenght() / R;
			const float finalAngleSlow = glm::radians(360.0f) * w * deltaTime;

			const float finalAngleFast = torque;

			const float finalAngle = glm::mix(finalAngleSlow, finalAngleFast, v.GetLenght() * 3);

			carTransform.RotateWorldSpace(finalAngleFast * deltaTime, { 0.0f, 1.0f, 0.0f });
		}

		cameraTransform.AddPosition(Vector3f(0, 0, 1) * cameraForwardMovement * deltaTime);
		// cameraTransform.AddPosition(cameraArmTransform.GetRightVector().GetNormalized() * cameraRightMovement * deltaTime);

		if (Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetAcceleration().GetLenght() > 0.5f * deltaTime ||
			glm::abs(Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetVelocity().Dot(carTransform.GetForwardVector())) > 1.2f * deltaTime) {
			const Vector2f flatCarVec = {
				Engine::GetEcs()->GetComponent<Transform3D>(carObject).GetRightVector().x,
				Engine::GetEcs()->GetComponent<Transform3D>(carObject).GetRightVector().Z
			};

			const Vector2f flatArmVec = {
				cameraArmTransform.GetForwardVector().x,
				cameraArmTransform.GetForwardVector().Z
			};

			const float angle = -flatArmVec.Dot(flatCarVec);
			if (cameraAttachedToCar) cameraRotation.x += angle * 750 * deltaTime;
		}

		cameraArmTransform.RotateWorldSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
		cameraArmTransform.RotateLocalSpace(glm::radians(cameraRotation.y * 0.25f), { 1, 0, 0 });

		camera.UpdateTransform(&cameraTransform);

		Engine::GetEcs()->GetComponent<CameraComponent2D>(cameraObject2d).UpdateUniformBuffer(
			Engine::GetEcs()->GetComponent<Transform2D>(cameraObject2d)
		);

		if (false) {
			const float targetFps = 30.0f;
			const float targetMs = 1.0f / targetFps;
			const auto waitMiliseconds = static_cast<long>((targetMs - deltaTime) * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(glm::max(0l, waitMiliseconds)));
		}

		// UI
		if (mouse) {
			const bool isPressed = mouse->GetMouseState().IsButtonDown(IO::MouseButton::BUTTON_LEFT);
			const Vector2f position = mouse->GetMouseState().GetPosition().ToVector2f();

			GetRootUiElement().UpdateByCursor(position, isPressed, Vector2f::Zero);
		}
	}

	void BuildFrame() override {
		const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

		auto graphicsCommandList = Engine::GetRenderer()->GetGraphicsCommandList();
		auto frameBuildCommandList = Engine::GetRenderer()->GetFrameBuildCommandList();
		auto renderpass = Engine::GetRenderer()->GetFinalRenderTarget();

		static SpriteRenderer spriteRenderer{};
		spriteRenderer.SetCommandList(graphicsCommandList);

		const Vector4ui windowRec = {
			0,
			0,
			Engine::GetDisplay()->GetResolution().x,
			Engine::GetDisplay()->GetResolution().y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		graphicsCommandList->SetViewport(viewport);
		graphicsCommandList->SetScissor(windowRec);

		// Render text
		graphicsCommandList->StartDebugSection("Text Rendering", Color::Blue);
		graphicsCommandList->BeginGraphicsRenderpass(textRenderTarget.GetPointer(), Color::Black * 0.0f);

		spriteRenderer.Begin();
		spriteRenderer.SetCamera(cameraObject2d);
		spriteRenderer.SetMaterial(*material2d);

		const auto& carTransform = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
		const auto& carPhysics = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);
		uiFpsText->SetText(
			// std::format("Pos: {} {} {}", carTransform.GetPosition().x, carTransform.GetPosition().y, carTransform.GetPosition().Z)
			std::format("FPS: {}", GetFps())
		);

		GetRootUiElement().Render(&spriteRenderer, Vector2f::Zero);

		Engine::GetConsole()->Draw(&spriteRenderer);

		// spriteRenderer.DrawString(font, 20,, Vector2f{ 50.0f }, Color::White);

		spriteRenderer.End();

		graphicsCommandList->EndGraphicsRenderpass();
		graphicsCommandList->EndDebugSection();

		// Full-screen rendering
		//
		// Pre-Effects
		frameBuildCommandList->StartDebugSection("Pre-Effects Frame build", Color(0, 1, 0));

		auto skyboxRenderSystemImg = Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->GetRenderTarget().GetMainColorImage(resourceIndex);
		auto renderSystemImg = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>()->GetRenderTarget().GetMainColorImage(resourceIndex);

		frameBuildCommandList->SetGpuImageBarrier(
			skyboxRenderSystemImg,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

		frameBuildCommandList->SetGpuImageBarrier(
			renderSystemImg,
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

		GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

		preEffectsFrameCombiner->Begin(frameBuildCommandList, FrameCombiner::ImageFormat::RGBA16);
		preEffectsFrameCombiner->Draw(frameBuildCommandList, *skyboxRenderSystemImg->GetView(viewConfig));
		preEffectsFrameCombiner->Draw(frameBuildCommandList, *renderSystemImg->GetView(viewConfig));
		preEffectsFrameCombiner->End(frameBuildCommandList);

		frameBuildCommandList->EndDebugSection();

		// Post-processing effects:
		if (config.useFxaa)
			fxaaPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		if (config.useBloom)
			bloomPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		toneMappingPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());

		// Frame build:
		frameBuildCommandList->StartDebugSection("Final Frame build", Color(0, 1, 0));
		auto colliderRenderSystem = Engine::GetEcs()->GetSystem<ECS::ColliderRenderSystem>();
		if (colliderRenderSystem) {
			frameBuildCommandList->SetGpuImageBarrier(
				colliderRenderSystem->GetRenderTarget().GetMainColorImage(resourceIndex),
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		}

		// Sync
		frameBuildCommandList->SetGpuImageBarrier(
			toneMappingPass->GetOutput().GetTargetImage(resourceIndex),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		frameBuildCommandList->SetGpuImageBarrier(
			textRenderTarget->GetMainColorImage(resourceIndex),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));

		frameBuildCommandList->BindVertexBuffer(*Sprite::globalVertexBuffer);
		frameBuildCommandList->BindIndexBuffer(*Sprite::globalIndexBuffer);

		frameBuildCommandList->SetViewport(viewport);
		frameBuildCommandList->SetScissor(windowRec);

		frameBuildCommandList->BeginGraphicsRenderpass(renderpass);
		frameBuildCommandList->BindMaterial(*Engine::GetRenderer()->GetFullscreenRenderingMaterial());

		frameBuildCommandList->BindMaterialSlot(*toneMappingPass->GetOutput().GetFullscreenSpriteMaterialSlot());
		frameBuildCommandList->DrawSingleInstance(6);

		frameBuildCommandList->BindMaterialSlot(*textRenderTarget->GetFullscreenSpriteMaterialSlot());
		frameBuildCommandList->DrawSingleInstance(6);

		if (colliderRenderSystem && colliderRenderSystem->IsActive()) {
			frameBuildCommandList->BindMaterialSlot(*colliderRenderSystem->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
			frameBuildCommandList->DrawSingleInstance(6);
		}

		frameBuildCommandList->EndGraphicsRenderpass();

		frameBuildCommandList->EndDebugSection();

		frameBuildCommandList->SetGpuImageBarrier(
			Engine::GetRenderer()->_GetSwapchain()->GetImage(resourceIndex),
			GpuImageLayout::COLOR_ATTACHMENT,
			GpuImageLayout::PRESENT,
			GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
			GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));
	}


	void OnWindowResize(const Vector2ui& size) override {
		textRenderTarget->Resize(size);
		preEffectsRenderTarget->Resize(size);
		fxaaPass->Resize(size);
		bloomPass->Resize(size);
		toneMappingPass->Resize(size);

		preEffectsFrameCombiner->Resize(size);

		SetupPostProcessingChain();
	}

	void OnExit() override {
		bloomPass.Delete();
		fxaaPass.Delete();
		toneMappingPass.Delete();

		for (auto& buffer : exposureBuffers)
			buffer.Delete();

		preEffectsRenderTarget.Delete();
		textRenderTarget.Delete();

		preEffectsFrameCombiner.Delete();
	}

private:

	OwnedPtr<UI::HorizontalContainer> CreateCheckbox(const std::string& text, UI::Button::CallbackFnc callback) {
		const static auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json", "GLOBAL")
			->GetTextureView2D();

		UI::HorizontalContainer* checkbox = new UI::HorizontalContainer(Vector2f(50.0f));

		auto button = new UI::Button(Vector2f(25.0f), "");
		button->GetSprite(UI::Button::State::DEFAULT).SetImageView(uiView);
		button->GetSprite(UI::Button::State::DEFAULT).color = Color::Red;

		button->GetSprite(UI::Button::State::PRESSED).SetImageView(uiView);
		button->GetSprite(UI::Button::State::PRESSED).color = Color::Green;

		button->GetSprite(UI::Button::State::SELECTED).SetImageView(uiView);
		button->GetSprite(UI::Button::State::SELECTED).color = Color::Blue;

		button->SetCallback(callback);
		button->SetState(UI::Button::State::PRESSED);

		auto textView = new UI::TextView(Vector2f(25.0f));
		textView->SetText(text);
		textView->SetFont(font);
		textView->SetFontSize(25);
		textView->AdjustSizeToText();

		checkbox->AddChild("button", button);
		checkbox->AddChild("text", textView);

		checkbox->AdjustSizeToChildren();
		checkbox->Rebuild();

		return checkbox;
	}

	void SetTaaState(bool) {
		auto renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		renderSystem->ToggleTaa();
	}

	void SetFxaaState(bool state) {
		config.useFxaa = state;
		SetupPostProcessingChain();
	}

	void SetBloomState(bool state) {
		config.useBloom = state;
		SetupPostProcessingChain();
	}

	void SetupRenderTargets() {
		// Text
		RenderTargetAttachmentInfo textColorInfo{ .format = Format::RGBA8_SRGB, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE, .name = "Text Color Target" };
		RenderTargetAttachmentInfo textDepthInfo{ .format = Format::D16_UNORM , .name = "Text Depth Target" };
		textRenderTarget = new RenderTarget();
		textRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { textColorInfo }, textDepthInfo);

		// Pre effects scene
		RenderTargetAttachmentInfo preEffectsColorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, .name = "Pre-Effects Color Target" };
		RenderTargetAttachmentInfo preEffectsDepthInfo{ .format = Format::D16_UNORM, .name = "Pre-Effects Depth Target" };
		preEffectsRenderTarget = new RenderTarget();
		preEffectsRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { preEffectsColorInfo }, preEffectsDepthInfo);

		// Combiners
		preEffectsFrameCombiner = new FrameCombiner();
		preEffectsFrameCombiner->Create(Engine::GetDisplay()->GetResolution(), preEffectsColorInfo);
	}

	void SetupPostProcessingChain() {
		// Inicializaciones
		if (!fxaaPass.HasValue()) {
			fxaaPass = new FxaaPass();
			fxaaPass->Create(Engine::GetDisplay()->GetResolution());
		}

		if (!bloomPass.HasValue()) {
			bloomPass = new BloomPass();
			bloomPass->Create(Engine::GetDisplay()->GetResolution());
		}

		if (!toneMappingPass.HasValue()) {
			toneMappingPass = new ToneMappingPass();
			toneMappingPass->Create(Engine::GetDisplay()->GetResolution());

			const GpuBuffer* epxBuffers[NUM_RESOURCES_IN_FLIGHT]{};
			for (UIndex32 i = 0; i < exposureBuffers.size(); i++) {
				exposureBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(float)).GetPointer();
				epxBuffers[i] = exposureBuffers[i].GetPointer();

				exposureBuffers[i]->MapMemory();
				exposureBuffers[i]->Write(toneMappingPass->GetExposure());
				exposureBuffers[i]->Unmap();
			}

			toneMappingPass->SetExposureBuffers(epxBuffers);
		}

		const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

		const auto& preEffectsSource = preEffectsFrameCombiner->GetRenderTarget();

		fxaaPass->SetInput(preEffectsSource, viewConfig);

		if (config.useBloom && config.useFxaa) {
			bloomPass->SetInput(fxaaPass->GetOutput(), viewConfig);
			toneMappingPass->SetInput(bloomPass->GetOutput(), viewConfig);
		} else
		if (config.useBloom) {
			bloomPass->SetInput(preEffectsSource, viewConfig);
			toneMappingPass->SetInput(bloomPass->GetOutput(), viewConfig);
		}
		else 
		if (config.useFxaa) {
			toneMappingPass->SetInput(fxaaPass->GetOutput(), viewConfig);
		}
		else {
			toneMappingPass->SetInput(preEffectsSource, viewConfig);
		}

		Engine::GetRenderer()->WaitForCompletion();

		fxaaPass->UpdateMaterialInstance();
		if (config.useBloom)
			bloomPass->UpdateMaterialInstance();
		toneMappingPass->UpdateMaterialInstance();
	}

	void SetupRenderSystems() {
		const CubemapTexture* skyboxTexture = Engine::GetAssetManager()->Load<CubemapTexture>("Resources/Assets/Skyboxes/skybox0.json", "GLOBAL");
		const SpecularMap* specularMap = Engine::GetAssetManager()->Load<SpecularMap>("Resources/Assets/IBL/irradiance0.json", "GLOBAL");
		const IrradianceMap* irradianceMap = Engine::GetAssetManager()->Load<IrradianceMap>("Resources/Assets/IBL/specular0.json", "GLOBAL");

		// PBR Render System
		OSK_CURRENT_RSYSTEM* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		renderSystem->Initialize(cameraObject, *irradianceMap, *specularMap);

		// Skybox Render System
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCamera(cameraObject);
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCubemap(*skyboxTexture);

		// Collider Render System
		Engine::GetEcs()->GetSystem<ColliderRenderSystem>()->Initialize(cameraObject);

	}

	void SpawnCamera() {
		cameraObject = Engine::GetEcs()->SpawnObject();
		cameraArmObject = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<Transform3D>(cameraArmObject, Transform3D(cameraArmObject));

		Transform3D* cameraTransform = &Engine::GetEcs()->AddComponent<Transform3D>(cameraObject, Transform3D(cameraObject));
		cameraTransform->AddPosition({ 0.0f, 0.1f, -1.1f });
		Engine::GetEcs()->AddComponent<CameraComponent3D>(cameraObject, {});

		cameraTransform->AttachToObject(cameraArmObject);
		Engine::GetEcs()->GetComponent<Transform3D>(cameraArmObject).AttachToObject(carObject); cameraAttachedToCar = true;
	}

	void SpawnCamera2D() {
		cameraObject2d = Engine::GetEcs()->SpawnObject();

		CameraComponent2D* camera2D = &Engine::GetEcs()->AddComponent<CameraComponent2D>(cameraObject2d, {});
		camera2D->LinkToDisplay(Engine::GetDisplay());

		Engine::GetEcs()->AddComponent<Transform2D>(cameraObject2d, Transform2D(cameraObject2d));
	}

	void SpawnCar() {
		carObject = Engine::GetEcs()->SpawnObject();

		// Setup del transform
		Transform3D& transform = Engine::GetEcs()->AddComponent<Transform3D>(carObject, ECS::Transform3D(carObject));
		// transform.AddPosition({0.0f, 80.0f, 0.0f});
		
		// Setup de físicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(carObject, {});
		physicsComponent.SetMass(4.0f);
		physicsComponent.centerOfMassOffset = Vector3f(0.0f, 0.17f * 0.5f, 0.0f);
		physicsComponent.localFrictionCoefficient = Vector3f(0.0f, 0.5f, 1.0f);

		// Setup de colisiones
		Collider collider{};

		OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 0.15f * 2, 0.17f, 0.35f * 2 }, 0));
		
		collider.SetTopLevelCollider(new SphereCollider(0.45f));
		collider.AddBottomLevelCollider(convexVolume.GetPointer());

		Engine::GetEcs()->AddComponent<Collider>(carObject, std::move(collider));

		// Setup del modelo 3D
		Model3D* carModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/mclaren.json", "GLOBAL");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(carObject, {});

		modelComponent->SetModel(carModel);
		modelComponent->SetMaterial(material);
		ModelLoader3D::SetupPbrModel(*carModel, modelComponent);
	}

	void SpawnCircuit() {
		circuitObject = Engine::GetEcs()->SpawnObject();

		// Transform
		Engine::GetEcs()->AddComponent<Transform3D>(circuitObject, Transform3D(circuitObject));

		// Modelo 3D
		Model3D* circuitModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/circuit0.json", "GLOBAL");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(circuitObject, {});

		modelComponent->SetModel(circuitModel); // animModel
		modelComponent->SetMaterial(material);
		ModelLoader3D::SetupPbrModel(*circuitModel, modelComponent);

		Collider collider{};
		OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 100.0f, 100.0f, 100.0f }, -100.0f));
		
		collider.SetTopLevelCollider(new AxisAlignedBoundingBox({ 100.0f, 5.0f, 100.0f }));
		collider.AddBottomLevelCollider(convexVolume.GetPointer());
		Engine::GetEcs()->AddComponent<Collider>(circuitObject, std::move(collider));

		PhysicsComponent physicsComponent{};
		physicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(circuitObject, std::move(physicsComponent));
	}

	void SpawnSecondCollider() {
		const GameObjectIndex secondObject = Engine::GetEcs()->SpawnObject();

		// Transform
		Transform3D* transform = &Engine::GetEcs()->AddComponent<Transform3D>(secondObject, Transform3D(secondObject));
		transform->AddPosition({ 0.5f, 0.0f, 0.0f });

		// Setup de físicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(secondObject, {});
		physicsComponent.SetMass(4.0f);
		physicsComponent.centerOfMassOffset = Vector3f::Zero;

		// Collider
		Collider collider{};

		OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 0.2f * 2, 0.2f * 2, 0.2f * 2 }, 0.0f));

		collider.SetTopLevelCollider(new AxisAlignedBoundingBox(0.95f));
		collider.AddBottomLevelCollider(convexVolume.GetPointer());

		Engine::GetEcs()->AddComponent<Collider>(secondObject, std::move(collider));
	}

	struct Config {
		bool useFxaa = true;
		bool useBloom = true;
	} config;

	UniquePtr<FrameCombiner> preEffectsFrameCombiner;

	UniquePtr<RenderTarget> preEffectsRenderTarget;
	UniquePtr<RenderTarget> textRenderTarget;

	UniquePtr<BloomPass> bloomPass;
	UniquePtr<FxaaPass> fxaaPass;
	UniquePtr<ToneMappingPass> toneMappingPass;

	OSK::GRAPHICS::Material* material = nullptr;
	OSK::GRAPHICS::Material* material2d = nullptr;
	OSK::GRAPHICS::Material* materialRenderTarget = nullptr;

	std::array<UniquePtr<GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> exposureBuffers{};

	UI::TextView* uiFpsText = nullptr;

	UI::HorizontalContainer* taaCheckbox = nullptr;
	UI::HorizontalContainer* fxaaCheckbox = nullptr;
	UI::HorizontalContainer* bloomCheckbox = nullptr;
	UI::HorizontalContainer* collisionCheckbox = nullptr;

	ECS::GameObjectIndex carObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex circuitObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraArmObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;

	ASSETS::Font* font = nullptr;

	AUDIO::Source source;

	bool cameraAttachedToCar = false;

};

#endif
