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
#include "UiDropdown.h"

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
#include "CollisionComponent.h"
#include "AxisAlignedBoundingBox.h"

#include "Math.h"

#include "RenderSystem3D.h"
#include "RenderSystem2D.h"
#include "HybridRenderSystem.h"
#include "RenderTarget.h"

#include "AudioSourceAl.h"

#include "DeferredRenderSystem.h"
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
#include "TreeNormalsRenderSystem.h"
#include "TreeNormalsPass.h"
#include "TreeGBufferPass.h"

#include "PreBuiltSpline3D.h"

#include <thread>

OSK::GRAPHICS::SpriteRenderer spriteRenderer;


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

#if defined(OSK_USE_FORWARD_RENDERER)
#define OSK_CURRENT_RSYSTEM OSK::ECS::RenderSystem3D
#elif defined(OSK_USE_DEFERRED_RENDERER)
#define OSK_CURRENT_RSYSTEM OSK::ECS::DeferredRenderSystem
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
			mouseInput->SetReturnMode(IO::MouseReturnMode::FREE);
			mouseInput->SetMotionMode(IO::MouseMotionMode::RAW);
		}
	}

	void SetupEngine() override {
		Engine::GetRenderer()->Initialize("Game", {}, *Engine::GetDisplay(), PresentMode::VSYNC_ON);
	}

	void OnCreate() override {
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
		font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/Fonts/font0.json");
		font->LoadSizedFont(22);
		font->SetMaterial(material2d);

		auto font = Engine::GetAssetManager()->Load<ASSETS::Font>("Resources/Assets/Fonts/font1.json");
		font->SetMaterial(material2d);

		Engine::GetConsole()->SetFont(font.GetAsset());

		SetupUi();

		// Audio test
		const auto sound = Engine::GetAssetManager()->Load<ASSETS::AudioAsset>("Resources/Assets/Audio/bounce_audio.json");
		source = OSK::Engine::GetAudioApi()->CreateNewSource().GetPointer();
		source->Initialize();
		source->As<AUDIO::AudioSourceAl>()->SetBuffer(sound->GetBuffer());
		source->SetLooping(true);
		source->SetGain(0.05f);
		source->Play();
	}

	void SetupUi() {
		auto logoContainer = new UI::HorizontalContainer({ 380.0f, 80.0f });

		const auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
			->GetTextureView2D();
		const auto iconView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json")
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

		// Dropdown
		auto* dropdown = new UI::Dropdown(Vector2f(160.0f, 40.0f));
		dropdown->SetAnchor(UI::Anchor::FULLY_CENTERED);
		dropdown->SetFont(font, 22);
		dropdown->SetSelectionCallback([](std::string_view t) {
			if (t == "VSYNC ON")
				Engine::GetRenderer()->SetPresentMode(PresentMode::VSYNC_ON);
			else
				Engine::GetRenderer()->SetPresentMode(PresentMode::VSYNC_ON_TRIPLE_BUFFER);
			});
		dropdown->SetBackground(uiView, Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.8f, 1.0f));
		dropdown->AddElement("VSYNC ON");
		dropdown->AddElement("VSYNC TRIPLE BUFFER");

		logoContainer->AddChild("dropdown", dropdown);

		logoContainer->SetAnchor(UI::Anchor::TOP | UI::Anchor::LEFT);

		logoContainer->AdjustSizeToChildren();
		logoContainer->Rebuild();
		
		GetRootUiElement().AddChild("", logoContainer);

		// Panel derecho
		UI::VerticalContainer* rightPanel = new UI::VerticalContainer(Vector2f(170.0f));
		rightPanel->SetKeepRelativeSize(true);

		rightPanel->GetSprite().SetImageView(uiView);
		rightPanel->GetSprite().color = Color(0.3f, 0.3f, 0.3f, 0.94f);
		rightPanel->SetAnchor(UI::Anchor::RIGHT | UI::Anchor::BOTTOM);
		rightPanel->SetPadding(Vector2f(6.0f));

		UI::TextView* rightPanelTitle = new UI::TextView(Vector2f(50.0f));
		rightPanelTitle->SetFont(font);
		rightPanelTitle->SetFontSize(27);
		rightPanelTitle->SetText("Settings");
		rightPanelTitle->AdjustSizeToText();
		rightPanelTitle->SetMargin(Vector4f(3.0f, 3.0f, 3.0f, 6.0f));

		rightPanel->AddChild("title", rightPanelTitle);

		taaCheckbox = CreateCheckbox("TAA", [this](bool isActive) { SetTaaState(isActive); }).GetPointer();
		fxaaCheckbox = CreateCheckbox("HBAO", [this](bool isActive) { SetHbaoState(isActive); }).GetPointer();
		bloomCheckbox = CreateCheckbox("Boom", [this](bool isActive) { SetBloomState(isActive); }).GetPointer();
		collisionCheckbox = CreateCheckbox("Show Collisions", [](bool) {
			Engine::GetEcs()->GetSystem<ECS::ColliderRenderSystem>()->ToggleActivationStatus();
			Engine::GetEcs()->GetSystem<ECS::RenderBoundsRenderer>()->ToggleActivationStatus();
			}).GetPointer();

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
		Engine::GetEcs()->RegisterSystem<ECS::RenderBoundsRenderer>(ECS::ISystem::DEFAULT_EXECUTION_ORDER);
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
			if (keyboard->IsKeyDown(IO::Key::LEFT_CONTROL)) {
				cameraForwardMovement *= 2.3f;
				cameraRightMovement *= 2.3f;
			}


			if (keyboard->IsKeyStroked(IO::Key::P)) {
				cameraAttachedToCar = false;
				cameraTransform.UnAttach();
			}

			if (keyboard->IsKeyStroked(IO::Key::M)) {
				mouse->SetReturnMode(IO::MouseReturnMode::ALWAYS_RETURN);
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
			isExecutingTestCases = true;
		}

		// Movimiento del coche

		Transform3D& carTransform = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
				

		PhysicsComponent& carPhysics = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);

		source->SetPitch(carPhysics.GetVelocity().GetLenght());


		constexpr static float ANGLE_PER_KEY = 45.0f;
		constexpr static float L = 1.5f;
		float currentAngle = 0.0f;

		if (keyboard) {
			
			// Rotación
			if (keyboard->IsKeyDown(IO::Key::LEFT)) {
				currentAngle += ANGLE_PER_KEY;
				carTransform.RotateWorldSpace(deltaTime * 2, { 0, 1, 0 });
				if (cameraAttachedToCar) cameraRotation.x += deltaTime * 250;
			}
			if (keyboard->IsKeyDown(IO::Key::RIGHT)) {
				currentAngle -= ANGLE_PER_KEY;
				carTransform.RotateWorldSpace(deltaTime * 2, { 0, -1, 0 });
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


		// Aplicación de movimiento y rotación de la cámara
		// camera.Rotate(-cameraRotation.X, cameraRotation.Y);

		
		cameraTransform.AddPosition(Vector3f(0, 0, 1) * cameraForwardMovement * deltaTime);
		if (!cameraAttachedToCar)
			cameraTransform.AddPosition(cameraTransform.GetRightVector().GetNormalized() * cameraRightMovement * deltaTime);

		if (Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetAcceleration().GetLenght() > 0.5f * deltaTime ||
			glm::abs(Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject).GetVelocity().Dot(carTransform.GetForwardVector())) > 1.2f * deltaTime) {
			const Vector2f flatCarVec = {
				Engine::GetEcs()->GetComponent<Transform3D>(carObject).GetRightVector().x,
				Engine::GetEcs()->GetComponent<Transform3D>(carObject).GetRightVector().z
			};

			const Vector2f flatArmVec = {
				cameraArmTransform.GetForwardVector().x,
				cameraArmTransform.GetForwardVector().z
			};

			const float angle = -flatArmVec.Dot(flatCarVec);
			if (cameraAttachedToCar) cameraRotation.x += angle * 750 * deltaTime;
		}

		if (cameraAttachedToCar) {
			cameraArmTransform.RotateWorldSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
			cameraArmTransform.RotateLocalSpace(glm::radians(cameraRotation.y * 0.25f), { 1, 0, 0 });
		}
		else {
			cameraTransform.RotateLocalSpace(glm::radians(-cameraRotation.x * 0.25f), { 0, 1, 0 });
			cameraTransform.RotateLocalSpace(glm::radians(cameraRotation.y * 0.25f), { 1, 0, 0 });
		}

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

		// Test Cases
		if (isExecutingTestCases) {
			const TDeltaTime timeSinceLastCase = Engine::GetCurrentTime() - lastTestCaseTime;
			
			if (timeSinceLastCase > 3.0f) {
				currentTestCase++;

				if (currentTestCase >= testCases.GetSize()) {
					isExecutingTestCases = false;
					currentTestCase = -1;
					goto _cont;
				}

				lastTestCaseTime = Engine::GetCurrentTime();
				Engine::GetLogger()->InfoLog(std::format("Test: {}", testCases[currentTestCase].name));
				Engine::GetConsole()->WriteLine(std::format("Test: {}", testCases[currentTestCase].name));
				
				auto& transformA = Engine::GetEcs()->GetComponent<Transform3D>(carObject);
				auto& transformB = Engine::GetEcs()->GetComponent<Transform3D>(carObject2);

				transformA.SetPosition(testCases[currentTestCase].positionA);
				transformB.SetPosition(testCases[currentTestCase].positionB);

				transformA.SetRotation({});
				transformB.SetRotation({});

				auto& physicsA = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject);
				auto& physicsB = Engine::GetEcs()->GetComponent<PhysicsComponent>(carObject2);

				physicsA._SetVelocity(currentTestCase % 16 < 8
					? Vector3f::Zero
					: testCases[currentTestCase].positionB - testCases[currentTestCase].positionA);
				physicsA._SetAcceleration(Vector3f::Zero);
				physicsA._SetAngularVelocity(Vector3f::Zero);

				physicsB._SetVelocity(currentTestCase % 16 >= 8
					? Vector3f::Zero
					: testCases[currentTestCase].positionA - testCases[currentTestCase].positionB);
				physicsB._SetAcceleration(Vector3f::Zero);
				physicsB._SetAngularVelocity(Vector3f::Zero);
			}

			for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
				if ((event.firstEntity == carObject || event.secondEntity == carObject) && (event.firstEntity == carObject2 || event.secondEntity == carObject2)) {
					Engine::GetLogger()->InfoLog(std::format("Collision: A: {}, B: {}, Position: {},{},{}",
						event.firstEntity, event.secondEntity,
						event.collisionInfo.GetSingleContactPoint().x,
						event.collisionInfo.GetSingleContactPoint().y,
						event.collisionInfo.GetSingleContactPoint().z));
				//	Engine::GetConsole()->WriteLine(std::format("Collision: A: {}, B: {}, Position: {},{},{}",
				//		event.firstEntity, event.secondEntity,
				//		event.collisionInfo.GetSingleContactPoint().x,
				//		event.collisionInfo.GetSingleContactPoint().y,
				//		event.collisionInfo.GetSingleContactPoint().Z));
				}
			}
		}
		
	_cont:

		// UI
		if (mouse) {
			const bool isPressed = mouse->GetMouseState().IsButtonDown(IO::MouseButton::BUTTON_LEFT);
			const Vector2f position = mouse->GetMouseState().GetPosition().ToVector2f();

			GetRootUiElement().UpdateByCursor(position, isPressed, Vector2f::Zero);
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

		auto skyboxRenderSystemImg = Engine::GetEcs()->GetSystem<ECS::SkyboxRenderSystem>()->GetRenderTarget().GetMainColorImage();
		auto renderSystemImg = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>()->GetRenderTarget().GetMainColorImage();

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
		if (config.useHbao)
			hbaoPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		if (config.useBloom)
			bloomPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());
		toneMappingPass->Execute(Engine::GetRenderer()->GetPostComputeCommandList());

		// Frame build:
		frameBuildCommandList->StartDebugSection("Final Frame build", Color(0, 1, 0));
		auto* colliderRenderSystem = Engine::GetEcs()->GetSystem<ECS::ColliderRenderSystem>();
		if (colliderRenderSystem) {
			frameBuildCommandList->SetGpuImageBarrier(
				colliderRenderSystem->GetRenderTarget().GetMainColorImage(),
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		}

		auto* boundsRenderSystem = Engine::GetEcs()->GetSystem<ECS::RenderBoundsRenderer>();
		if (boundsRenderSystem) {
			frameBuildCommandList->SetGpuImageBarrier(
				boundsRenderSystem->GetRenderTarget().GetMainColorImage(),
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		}

		// Sync
		frameBuildCommandList->SetGpuImageBarrier(
			toneMappingPass->GetOutput().GetTargetImage(),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ));
		frameBuildCommandList->SetGpuImageBarrier(
			textRenderTarget->GetMainColorImage(),
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

		if (boundsRenderSystem && boundsRenderSystem->IsActive()) {
			/*frameBuildCommandList->BindMaterialSlot(*boundsRenderSystem->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
			frameBuildCommandList->DrawSingleInstance(6);*/
		}

		frameBuildCommandList->EndGraphicsRenderpass();

		frameBuildCommandList->EndDebugSection();

		frameBuildCommandList->SetGpuImageBarrier(
			Engine::GetRenderer()->_GetSwapchain()->GetImage(Engine::GetRenderer()->GetCurrentResourceIndex()),
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

		SetupTreeNormals();
	}

	void OnExit() override {
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

	void SetupTreeNormals() {
		auto* renderSystem = Engine::GetEcs()->GetSystem<DeferredRenderSystem>();
		auto* treeRenderSystem = Engine::GetEcs()->GetSystem<TreeNormalsRenderSystem>();

		const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

		auto* treeGBufferPass = renderSystem->GetRenderPass(TreeGBufferPass::GetRenderPassName())->As<TreeGBufferPass>();
		treeGBufferPass->GetMaterialInstance()->GetSlot("normals")->SetGpuImage("preCalculatedNormalTexture",
			treeRenderSystem->GetRenderTarget().GetMainColorImage()->GetView(viewConfig));
		treeGBufferPass->GetMaterialInstance()->GetSlot("normals")->FlushUpdate();
	}

	OwnedPtr<UI::HorizontalContainer> CreateCheckbox(const std::string& text, UI::Button::CallbackFnc callback) {
		const static auto uiView = &Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/button_texture.json")
			->GetTextureView2D();

		UI::HorizontalContainer* checkbox = new UI::HorizontalContainer(Vector2f(50.0f));
		checkbox->SetKeepRelativeSize(true);

		auto button = new UI::Button(Vector2f(25.0f), "");
		button->SetKeepRelativeSize(true);

		button->GetSprite(UI::Button::State::DEFAULT).SetImageView(uiView);
		button->GetSprite(UI::Button::State::DEFAULT).color = Color::Red;

		button->GetSprite(UI::Button::State::PRESSED).SetImageView(uiView);
		button->GetSprite(UI::Button::State::PRESSED).color = Color::Green;

		button->GetSprite(UI::Button::State::SELECTED).SetImageView(uiView);
		button->GetSprite(UI::Button::State::SELECTED).color = Color::Blue;

		button->SetCallback(callback);
		button->SetState(UI::Button::State::PRESSED);

		auto textView = new UI::TextView(Vector2f(25.0f));
		textView->SetKeepRelativeSize(true);

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
		textRenderTarget = new RenderTarget();
		textRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { textColorInfo }, textDepthInfo);

		// Pre effects scene
		RenderTargetAttachmentInfo preEffectsColorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE, .name = "Pre-Effects Color Target" };
		RenderTargetAttachmentInfo preEffectsDepthInfo{ .format = Format::D16_UNORM, .name = "Pre-Effects Depth Target" };
		preEffectsRenderTarget = new RenderTarget();
		preEffectsRenderTarget->Create(Engine::GetDisplay()->GetResolution(), { preEffectsColorInfo }, preEffectsDepthInfo);

		// Combiners
		preEffectsFrameCombiner = new FrameCombiner();
		preEffectsFrameCombiner->Create(Engine::GetDisplay()->GetResolution(), preEffectsColorInfo);
	}

	void SetupPostProcessingChain() {
		// Inicializaciones
		if (!hbaoPass.HasValue()) {
			hbaoPass = new HbaoPass();
			hbaoPass->Create(Engine::GetDisplay()->GetResolution());
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

		auto& preEffectsSource = preEffectsFrameCombiner->GetRenderTarget();

		auto* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();

		if (config.useHbao) {
			hbaoPass->SetInputTarget(preEffectsSource, viewConfig);
			hbaoPass->SetNormalsInput(renderSystem->GetGbuffer().GetImage(GBuffer::Target::NORMAL));
			hbaoPass->SetDepthInput(renderSystem->GetGbuffer().GetImage(GBuffer::Target::DEPTH));
		}

		if (config.useBloom && config.useHbao) {
			bloomPass->SetInputTarget(hbaoPass->GetOutput(), viewConfig);
			toneMappingPass->SetInputTarget(bloomPass->GetOutput(), viewConfig);
		} else
		if (config.useBloom) {
			bloomPass->SetInputTarget(preEffectsSource, viewConfig);
			toneMappingPass->SetInputTarget(bloomPass->GetOutput(), viewConfig);
		}
		else 
		if (config.useHbao) {
			toneMappingPass->SetInputTarget(hbaoPass->GetOutput(), viewConfig);
		}
		else {
			toneMappingPass->SetInputTarget(preEffectsSource, viewConfig);
		}

		Engine::GetRenderer()->WaitForCompletion();

		if (config.useHbao)
			hbaoPass->UpdateMaterialInstance();
		if (config.useBloom)
			bloomPass->UpdateMaterialInstance();
		toneMappingPass->UpdateMaterialInstance();
	}

	void SetupRenderSystems() {
		auto skyboxTexture = Engine::GetAssetManager()->Load<CubemapTexture>("Resources/Assets/Skyboxes/skybox0.json");
		auto specularMap = Engine::GetAssetManager()->Load<SpecularMap>("Resources/Assets/IBL/irradiance0.json");
		auto irradianceMap = Engine::GetAssetManager()->Load<IrradianceMap>("Resources/Assets/IBL/specular0.json");

		// PBR Render System
		OSK_CURRENT_RSYSTEM* renderSystem = Engine::GetEcs()->GetSystem<OSK_CURRENT_RSYSTEM>();
		renderSystem->Initialize(cameraObject, irradianceMap, specularMap);

		auto* treeRenderSystem = Engine::GetEcs()->GetSystem<TreeNormalsRenderSystem>();
		treeRenderSystem->Initialize(cameraObject);

		SetupTreeNormals();


		// Skybox Render System
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCamera(cameraObject);
		Engine::GetEcs()->GetSystem<SkyboxRenderSystem>()->SetCubemap(skyboxTexture);

		// Debug Render Systems
		Engine::GetEcs()->GetSystem<ColliderRenderSystem>()->Initialize(cameraObject);
		Engine::GetEcs()->GetSystem<RenderBoundsRenderer>()->Initialize(cameraObject);

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
		{
			carObject = Engine::GetEcs()->SpawnObject();
			Engine::GetLogger()->InfoLog(std::format("\tCar: {}", carObject));

			// Setup del transform
			Transform3D& transform = Engine::GetEcs()->AddComponent<Transform3D>(carObject, ECS::Transform3D(carObject));
			transform.SetPosition({ 0.0f, 1.75f, -9.5f});

			// Setup de físicas
			auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(carObject, {});
			physicsComponent.SetMass(4.0f);
			physicsComponent.centerOfMassOffset = Vector3f(0.0f, 0.17f * 0.5f, 0.0f);

			// Setup de colisiones
			CollisionComponent collider{};
			collider.SetCollider(Collider());

			OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 0.15f * 2, 0.17f, 0.35f * 2 }));
			convexVolume->MergeFaces();

			collider.GetCollider()->SetTopLevelCollider(new SphereCollider(0.45f));
			collider.GetCollider()->AddBottomLevelCollider(convexVolume.GetPointer());

			Engine::GetEcs()->AddComponent<CollisionComponent>(carObject, std::move(collider));

			// Setup del modelo 3D
			auto carModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/ow.json");

			ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(carObject, {});

			modelComponent->SetModel(carModel);
		}

		// 2
		{
			carObject2 = Engine::GetEcs()->SpawnObject();

			// Setup del transform
			Transform3D& transform = Engine::GetEcs()->AddComponent<Transform3D>(carObject2, ECS::Transform3D(carObject2));
			transform.SetPosition({ 0.0f, 2.25f, -11.5f });

			// Setup de físicas
			auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(carObject2, {});
			physicsComponent.SetMass(4.0f);
			physicsComponent.centerOfMassOffset = Vector3f(0.0f, 0.17f * 0.5f, 0.0f);

			// Setup de colisiones
			CollisionComponent collider{};
			collider.SetCollider(Collider());

			OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 0.15f * 2, 0.17f, 0.35f * 2 }));

			collider.GetCollider()->SetTopLevelCollider(new SphereCollider(0.45f));
			collider.GetCollider()->AddBottomLevelCollider(convexVolume.GetPointer());

			Engine::GetEcs()->AddComponent<CollisionComponent>(carObject2, std::move(collider));

			// Setup del modelo 3D
			auto carModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/mclaren.json");

			ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(carObject2, {});

			modelComponent->SetModel(carModel);
		}
	}

	void SpawnCircuit() {
		circuitObject = Engine::GetEcs()->SpawnObject();
		Engine::GetLogger()->InfoLog(std::format("\tCircuit: {}", circuitObject));

		// Transform
		Engine::GetEcs()->AddComponent<Transform3D>(circuitObject, Transform3D(circuitObject));

		// Modelo 3D
		auto circuitModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/circuit0.json");

		ModelComponent3D* modelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(circuitObject, {});

		modelComponent->SetModel(circuitModel); // animModel


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
		Engine::GetEcs()->AddComponent<PhysicsComponent>(circuitObject, std::move(physicsComponent));

		// Billboards
		const auto billboards = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<Transform3D>(billboards, Transform3D(billboards));

		auto billboardsModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/circuit0_billboards.json");

		ModelComponent3D* billboardsModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(billboards, {});

		billboardsModelComponent->SetModel(billboardsModel); // animModel
		PhysicsComponent billboardsPhysicsComponent{};
		billboardsPhysicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(billboards, std::move(billboardsPhysicsComponent));

		// Tree normals
		const auto treeNormals = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<Transform3D>(treeNormals, Transform3D(treeNormals));

		auto treeNormalsModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/circuit0_tree_normals.json");

		ModelComponent3D* treeNormalsModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(treeNormals, {});

		treeNormalsModelComponent->SetModel(treeNormalsModel); // animModel
		treeNormalsModelComponent->SetCastShadows(false);

		PhysicsComponent treeNormalsPhysicsComponent{};
		treeNormalsPhysicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(treeNormals, std::move(treeNormalsPhysicsComponent));

		// Trees
		const auto trees = Engine::GetEcs()->SpawnObject();

		Engine::GetEcs()->AddComponent<Transform3D>(trees, Transform3D(trees));

		auto treesModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/circuit0_trees.json");

		ModelComponent3D* treesModelComponent = &Engine::GetEcs()->AddComponent<ModelComponent3D>(trees, {});

		treesModelComponent->SetModel(treesModel); // animModel
		treesModelComponent->SetCastShadows(false);

		PhysicsComponent treesPhysicsComponent{};
		treesPhysicsComponent.SetImmovable();
		Engine::GetEcs()->AddComponent<PhysicsComponent>(trees, std::move(treesPhysicsComponent));

		// Spline test
		auto spline = Engine::GetAssetManager()->Load<PreBuiltSpline3D>("Resources/Assets/Curves/circuit0.json");
		auto& loadedSpline = spline->Get();
	}

	void SpawnSecondCollider() {
		return;

		const GameObjectIndex secondObject = Engine::GetEcs()->SpawnObject();

		// Transform
		Transform3D* transform = &Engine::GetEcs()->AddComponent<Transform3D>(secondObject, Transform3D(secondObject));
		transform->AddPosition({ 0.5f, 0.0f, 0.0f });

		// Setup de físicas
		auto& physicsComponent = Engine::GetEcs()->AddComponent<PhysicsComponent>(secondObject, {});
		physicsComponent.SetMass(2.0f);
		physicsComponent.centerOfMassOffset = Vector3f::Zero;

		// Collider
		CollisionComponent collider{};
		collider.SetCollider(Collider());

		OwnedPtr<ConvexVolume> convexVolume = new ConvexVolume(ConvexVolume::CreateObb({ 0.2f * 2, 0.2f * 2, 0.2f * 2 }));

		collider.GetCollider()->SetTopLevelCollider(new AxisAlignedBoundingBox(Vector3f(0.95f)));
		collider.GetCollider()->AddBottomLevelCollider(convexVolume.GetPointer());

		Engine::GetEcs()->AddComponent<CollisionComponent>(secondObject, std::move(collider));
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

	bool isExecutingTestCases = false;
	TDeltaTime lastTestCaseTime = 0.0f;
	int currentTestCase = -1;

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

	std::array<UniquePtr<GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> exposureBuffers{};

	UI::TextView* uiFpsText = nullptr;

	UI::HorizontalContainer* taaCheckbox = nullptr;
	UI::HorizontalContainer* fxaaCheckbox = nullptr;
	UI::HorizontalContainer* bloomCheckbox = nullptr;
	UI::HorizontalContainer* collisionCheckbox = nullptr;

	ECS::GameObjectIndex carObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex carObject2 = ECS::EMPTY_GAME_OBJECT;

	ECS::GameObjectIndex circuitObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraArmObject = ECS::EMPTY_GAME_OBJECT;
	ECS::GameObjectIndex cameraObject2d = ECS::EMPTY_GAME_OBJECT;

	ASSETS::AssetRef<ASSETS::Font> font;

	UniquePtr<AUDIO::IAudioSource> source;

	bool cameraAttachedToCar = false;

};

#endif
