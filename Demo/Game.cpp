#include "Game.h"

#include <OSKengine/OSKengine.h>

#include <OSKengine/IKeyboardInput.h>
#include <OSKengine/IGamepadInput.h>
#include <OSKengine/IMouseInput.h>
#include <OSKengine/MouseModes.h>

#include <OSKengine/IDisplay.h>
#include <OSKengine/Viewport.h>
#include <OSKengine/DeferredRenderSystem.h>
#include <OSKengine/TreeNormalsRenderSystem.h>
#include <OSKengine/TreeNormalsPass.h>
#include <OSKengine/TreeGBufferPass.h>
#include <OSKengine/ColliderRenderer.h>
#include <OSKengine/SkyboxRenderSystem.h>
#include <OSKengine/SpriteRenderer.h>

#include <OSKengine/PhysicsSystem.h>
#include <OSKengine/CollisionSystem.h>
#include <OSKengine/PhysicsResolver.h>

#include <OSKengine/CameraComponent2D.h>
#include <OSKengine/CameraComponent3D.h>
#include <OSKengine/Transform2D.h>
#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include <OSKengine/AssetManager.h>
#include <OSKengine/IrradianceMap.h>
#include <OSKengine/SpecularMap.h>
#include <OSKengine/CubemapTexture.h>
#include <OSKengine/Texture.h>
#include <OSKengine/Font.h>
#include <OSKengine/PreBuiltSpline3D.h>

#include <OSKengine/IUiContainer.h>
#include <OSKengine/UiHorizontalContainer.h>
#include <OSKengine/UiVerticalContainer.h>
#include <OSKengine/UiImageView.h>
#include <OSKengine/UiTextView.h>
#include <OSKengine/UiButton.h>

#include <OSKengine/IFullscreenableDisplay.h>

#include "CarComponent.h"
#include "EngineComponent.h"
#include "CameraArmComponent.h"
#include "CircuitComponent.h"
#include "CarAiComponent.h"
#include "CarControllerComponent.h"

#include "CarInputEvent.h"
#include "CarInputSystem.h"
#include "CarSystem.h"
#include "CameraAttachmentSystem.h"
#include "CarAssetsLoader.h"
#include "CarRegistry.h"
#include "HubCameraSystem.h"
#include "CarAiSystem.h"
#include "RayRenderSystem.h"

#include "MainMenu.h"
#include "CarSelectPanel.h"
#include "MenuEvents.h"

#include "UiBuilder.h"
#include "RayCastEvent.h"

void Game::CreateWindow() {
	OSK::Engine::GetDisplay()->Create(
		{ 1280u, 720u }, 
		"OSKengine Demo");
	
	OSK::IO::IMouseInput* mouseInput = nullptr;
	OSK::Engine::GetInput()->QueryInterface(
		OSK::IUUID::IMouseInput, 
		(void**)&mouseInput
	);

	if (mouseInput) {
		using MouseReturnMode = OSK::IO::MouseReturnMode;
		using MouseMotionMode = OSK::IO::MouseMotionMode;

		mouseInput->SetReturnMode(MouseReturnMode::FREE);
		mouseInput->SetMotionMode(MouseMotionMode::RAW);
	}
}

void Game::SetupEngine() {
	OSK::Engine::GetRenderer()->Initialize(
		"OSKengine Demo",
		OSK::Version({ .mayor = 0u, .menor = 0u, .parche = 0u }),
		*OSK::Engine::GetDisplay(),
		OSK::GRAPHICS::PresentMode::VSYNC_ON
	);
}

void Game::OnCreate() {
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	RegisterEcse();

	CarRegistry::LoadRegistry();

	SpanwCamera();

	ecs->RegisterSystem<OSK::ECS::ColliderRenderSystem>(0)->Initialize(cameraObject);

	ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->Initialize(
		cameraObject,
		assetsManager->Load<OSK::ASSETS::IrradianceMap>("Resources/Assets/IBL/irradiance0.json"),
		assetsManager->Load<OSK::ASSETS::SpecularMap>("Resources/Assets/IBL/specular0.json")
	);
	ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.x = 0.01f;
	ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.y = -1.0f;
	ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.Z = 0.01f;
	ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.W = 1.0f;

	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetIblConfig().emissiveStrength= 1.0f;

	auto* renderSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>();
	std::array<OSK::GRAPHICS::GpuImage*, NUM_RESOURCES_IN_FLIGHT> depthImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		depthImages[i] = renderSystem->GetGbuffer().GetImage(i, OSK::GRAPHICS::GBuffer::Target::DEPTH);

	OSK::Engine::GetEcs()->GetSystem<RayRenderSystem>()->SetDepthImages(depthImages);

	ecs->GetSystem<OSK::ECS::TreeNormalsRenderSystem>()->Initialize(cameraObject);
	SetupTreeNormals();

	ecs->GetSystem<OSK::ECS::SkyboxRenderSystem>()->SetCamera(cameraObject);
	ecs->GetSystem<OSK::ECS::SkyboxRenderSystem>()->SetCubemap(
		assetsManager->Load<OSK::ASSETS::CubemapTexture>("Resources/Assets/Skyboxes/skybox0.json")
	);

	cameraObject2d = ecs->SpawnObject();

	OSK::ECS::CameraComponent2D* camera2D = &ecs->AddComponent<OSK::ECS::CameraComponent2D>(cameraObject2d, {});
	camera2D->LinkToDisplay(OSK::Engine::GetDisplay());

	ecs->AddComponent<OSK::ECS::Transform2D>(cameraObject2d, OSK::ECS::Transform2D(cameraObject2d));

	finalFrameCombiner = new OSK::GRAPHICS::FrameCombiner();

	OSK::GRAPHICS::RenderTargetAttachmentInfo finalFrameCombinerConfig =  { 
		.format = OSK::GRAPHICS::Format::RGBA16_SFLOAT, 
		.usage = OSK::GRAPHICS::GpuImageUsage::SAMPLED | OSK::GRAPHICS::GpuImageUsage::COMPUTE | OSK::GRAPHICS::GpuImageUsage::TRANSFER_SOURCE,
		.name = "Final Image" 
	};
	finalFrameCombiner->Create(OSK::Engine::GetDisplay()->GetResolution(), finalFrameCombinerConfig);

	using Format = OSK::GRAPHICS::Format;
	using GpuImageUsage = OSK::GRAPHICS::GpuImageUsage;

	textRenderTarget = new OSK::GRAPHICS::RenderTarget();
	const OSK::GRAPHICS::RenderTargetAttachmentInfo colorInfo = {
		.format = Format::RGBA8_SRGB,
		.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED
	};
	const OSK::GRAPHICS::RenderTargetAttachmentInfo depthInfo = {
		.format = Format::D16_UNORM,
		.usage = GpuImageUsage::DEPTH
	};
	textRenderTarget->Create(
		OSK::Engine::GetDisplay()->GetResolution(),
		{ colorInfo },
		depthInfo
	);

	material2d = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json");
	material3d = OSK::Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/direct_pbr.json");
		
	std::array<OSK::GRAPHICS::GpuImage*, NUM_RESOURCES_IN_FLIGHT> normalImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		normalImages[i] = renderSystem->GetGbuffer().GetImage(i, OSK::GRAPHICS::GBuffer::Target::NORMAL);

	hbaoPass = new OSK::GRAPHICS::HbaoPass();
	hbaoPass->Create(OSK::Engine::GetDisplay()->GetResolution());

	bloomPass = new OSK::GRAPHICS::BloomPass();
	bloomPass->Create(OSK::Engine::GetDisplay()->GetResolution());

	toneMappingPass = new OSK::GRAPHICS::ToneMappingPass();
	toneMappingPass->Create(OSK::Engine::GetDisplay()->GetResolution());

	SetupPostProcessingChain();

	SetupUi();

	carSpawner.SetMaterial3D(material3d);

	// firstCar = carSpawner.Spawn(OSK::Vector3f(0.0f, 0.0f, 10.0f), CarRegistry::GetAssetsRoute("LWM 0"));

	m_hub.Spawn(&carSpawner, CarRegistry::GetAssetsRoute("LWM 0"));
	firstCar = m_hub.GetCarObject();

	circuitSpawner.SetMaterial3D(material3d);
	// circuitSpawner.Spawn();

	SetMainCar(firstCar);

	//
	OSK::Engine::GetLogger()->InfoLog(std::format("VRAM: {} Mb", OSK::Engine::GetRenderer()->GetAllocator()->GetMemoryUsageInfo().gpuOnlyMemoryInfo.usedSpace / 1000000));
}

void Game::SetupPostProcessingChain() {
	auto* renderSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>();
	std::array<OSK::GRAPHICS::GpuImage*, NUM_RESOURCES_IN_FLIGHT> depthImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		depthImages[i] = renderSystem->GetGbuffer().GetImage(i, OSK::GRAPHICS::GBuffer::Target::DEPTH);

	std::array<OSK::GRAPHICS::GpuImage*, NUM_RESOURCES_IN_FLIGHT> normalImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		normalImages[i] = renderSystem->GetGbuffer().GetImage(i, OSK::GRAPHICS::GBuffer::Target::NORMAL);

	hbaoPass->SetInputTarget(finalFrameCombiner->GetRenderTarget(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	hbaoPass->SetNormalsInput(normalImages);
	hbaoPass->SetDepthInput(depthImages);

	hbaoPass->UpdateMaterialInstance();

	bloomPass->SetInputTarget(hbaoPass->GetOutput(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	bloomPass->UpdateMaterialInstance();

	toneMappingPass->SetExposure(7.5f);

	toneMappingPass->SetInputTarget(bloomPass->GetOutput(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	toneMappingPass->UpdateMaterialInstance();
}

void Game::OnTick(TDeltaTime deltaTime) {
	const OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	const OSK::IO::IKeyboardInput* keyboard = nullptr;
	const OSK::IO::IGamepadInput*  gamepad  = nullptr;
	const OSK::IO::IMouseInput*    mouse	= nullptr;

	// Obtenemos las interfaces necesarias
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IKeyboardInput, (const void**)&keyboard);
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IGamepadInput,	 (const void**)&gamepad);
	OSK::Engine::GetInput()->QueryConstInterface(OSK::IUUID::IMouseInput,	 (const void**)&mouse);

	if (keyboard && keyboard->IsKeyDown(OSK::IO::Key::ESCAPE))
		ToMainMenu();

	if (keyboard && keyboard->IsKeyStroked(OSK::IO::Key::R)) {
		OSK::Engine::GetRenderer()->GetMaterialSystem()->ReloadAllMaterials();
	}

	if (keyboard && keyboard->IsKeyStroked(OSK::IO::Key::F11)) {
		OSK::IO::IFullscreenableDisplay* display = nullptr;
		OSK::Engine::GetDisplay()->QueryInterface(OSK::IUUID::IFullscreenableDisplay, (void**)&display);

		if (display) {
			display->ToggleFullscreen();
		}
	}

	OSK::Vector2f cameraRotation = OSK::Vector2f::Zero;

	if (mouse) {
		GetRootUiElement().UpdateByCursor(
			mouse->GetMouseState().GetPosition().ToVector2f(),
			mouse->GetMouseState().IsButtonDown(OSK::IO::MouseButton::BUTTON_LEFT),
			OSK::Vector2f::Zero);
	}

	OSK::ECS::CameraComponent3D& camera = ecs->GetComponent<OSK::ECS::CameraComponent3D>(cameraObject);
	OSK::ECS::Transform3D& cameraTransform = ecs->GetComponent<OSK::ECS::Transform3D>(cameraObject);

	OSK::ECS::Transform3D& cameraArmTransform = ecs->GetComponent<OSK::ECS::Transform3D>(cameraArmObject);

	camera.UpdateTransform(&cameraTransform);
	hbaoPass->UpdateCamera(glm::inverse(camera.GetProjectionMatrix()), camera.GetViewMatrix(cameraTransform), camera.GetNearPlane());

	if (m_startNextFrame) {
		UnpauseSystems();
		m_startNextFrame = false;
	}

	CheckEvents();


	// GUI
	const auto& motor = ecs->GetComponent<EngineComponent>(firstCar);
	const auto& physics = ecs->GetComponent<OSK::ECS::PhysicsComponent>(firstCar);
	gearText->SetText(std::format("Gear: {}", motor.currentGear + 1));
	rpmText->SetText(std::format("RPM: {:.0f}", motor.currentRpm));
	speedText->SetText(std::format("Vel.: {:.0f} km/h", physics.GetVelocity().GetLenght() / 1000.0f * 3600.0f));

	collisionTesting.Update();
}

void Game::CheckEvents() {
	auto* ecs = OSK::Engine::GetEcs();

	if (!ecs->GetEventQueue<ExitEvent>().IsEmpty()) {
		Exit();
	}

	if (!ecs->GetEventQueue<LoadLevelEvent>().IsEmpty()) {
		GetRootUiElement().GetChild(MainMenu::Name)->SetInvisible();
		ToGame(ecs->GetEventQueue<LoadLevelEvent>()[0].assetsPath);
	}

	// Selección de coche
	for (const auto& e : ecs->GetEventQueue<SelectCarEvent>()) {
		m_hub.SetCar(&carSpawner, e.assetsPath);
	}

	// Selección de coche
	for (const auto& e : ecs->GetEventQueue<OSK::ECS::CollisionEvent>()) {
		if (e.firstEntity != firstCar || e.secondEntity || firstCar) {
			continue;
		}

		const auto distanceToCar = e.collisionInfo.GetSingleContactPoint().GetDistanceTo(
			OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(firstCar).GetPosition());

		if (distanceToCar > 1.0f) {
			OSK::Engine::GetLogger()->InfoLog(std::format("!! Distance: {:.2}", distanceToCar));
		}
	}
}

void Game::OnExit() {
	auto* ecs = OSK::Engine::GetEcs();
	OSK::Engine::GetLogger()->InfoLog(std::format("{}", ecs->EventHasBeenRegistered<PlayEvent>()));

	m_hub.Unspawn();

	finalFrameCombiner.Delete();
	textRenderTarget.Delete();

	hbaoPass.Delete();
	bloomPass.Delete();
	toneMappingPass.Delete();
}

void Game::OnWindowResize(const OSK::Vector2ui& newRes) {
	if (finalFrameCombiner.HasValue())
		finalFrameCombiner->Resize(newRes);
	if (textRenderTarget.HasValue())
		textRenderTarget->Resize(newRes);

	hbaoPass->Resize(newRes);
	bloomPass->Resize(newRes);
	toneMappingPass->Resize(newRes);

	SetupPostProcessingChain();

	SetupTreeNormals();

	auto* renderSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>();
	std::array<OSK::GRAPHICS::GpuImage*, NUM_RESOURCES_IN_FLIGHT> depthImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		depthImages[i] = renderSystem->GetGbuffer().GetImage(i, OSK::GRAPHICS::GBuffer::Target::DEPTH);

	OSK::Engine::GetEcs()->GetSystem<RayRenderSystem>()->SetDepthImages(depthImages);
}

void Game::BuildFrame() {
	OSK::GRAPHICS::IRenderer* renderer = OSK::Engine::GetRenderer();
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	const auto resourceIndex = renderer->GetCurrentResourceIndex();

	OSK::GRAPHICS::ICommandList* graphicsCommandList	= renderer->GetGraphicsCommandList();
	OSK::GRAPHICS::ICommandList* frameBuildCommandList	= renderer->GetFrameBuildCommandList();
	OSK::GRAPHICS::ICommandList* computeCommandList		= renderer->GetPostComputeCommandList();
	OSK::GRAPHICS::RenderTarget* renderTarget			= renderer->GetFinalRenderTarget();

	OSK::GRAPHICS::Viewport viewport = {
		.rectangle = OSK::Vector4ui(
			0u,
			0u,
			OSK::Engine::GetDisplay()->GetResolution().x,
			OSK::Engine::GetDisplay()->GetResolution().y
		)
	};

	graphicsCommandList->BeginGraphicsRenderpass(textRenderTarget.GetPointer(), OSK::Color::Black * 0.0f);
	
	graphicsCommandList->SetViewport(viewport);
	graphicsCommandList->SetScissor(viewport.rectangle);
	
	OSK::GRAPHICS::SpriteRenderer spriteRenderer{};
	spriteRenderer.SetCommandList(graphicsCommandList);
	
	spriteRenderer.Begin();
	spriteRenderer.SetCamera(cameraObject2d);
	spriteRenderer.SetMaterial(*material2d);

	GetRootUiElement().Render(&spriteRenderer, OSK::Vector2f::Zero);

	spriteRenderer.End();

	graphicsCommandList->EndGraphicsRenderpass();

	// -- //
	using GpuImageLayout = OSK::GRAPHICS::GpuImageLayout;
	using GpuCommandStage = OSK::GRAPHICS::GpuCommandStage;
	using GpuAccessStage = OSK::GRAPHICS::GpuAccessStage;

	OSK::GRAPHICS::GpuImage* skyboxRenderSystemImg 
		= ecs->GetSystem<OSK::ECS::SkyboxRenderSystem>()->GetRenderTarget().GetMainColorImage(resourceIndex);
	OSK::GRAPHICS::GpuImage* sceneRenderSystemImg
		= ecs->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetRenderTarget().GetMainColorImage(resourceIndex);
	OSK::GRAPHICS::GpuImage* rayRenderSystemImg
		= ecs->GetSystem<RayRenderSystem>()->GetRenderTarget().GetMainColorImage(resourceIndex);

	frameBuildCommandList->SetGpuImageBarrier(skyboxRenderSystemImg,
		GpuImageLayout::SAMPLED,
		OSK::GRAPHICS::GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	finalFrameCombiner->Begin(frameBuildCommandList, OSK::GRAPHICS::FrameCombiner::ImageFormat::RGBA16);
	finalFrameCombiner->Draw(frameBuildCommandList, *skyboxRenderSystemImg->GetView(OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	finalFrameCombiner->Draw(frameBuildCommandList, *sceneRenderSystemImg->GetView(OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	finalFrameCombiner->End(frameBuildCommandList);

	hbaoPass->Execute(computeCommandList);
	bloomPass->Execute(computeCommandList);
	toneMappingPass->Execute(computeCommandList);

	frameBuildCommandList->BeginGraphicsRenderpass(renderTarget);

	frameBuildCommandList->SetViewport(viewport);
	frameBuildCommandList->SetScissor(viewport.rectangle);

	frameBuildCommandList->BindVertexBuffer(*OSK::GRAPHICS::Sprite::globalVertexBuffer);
	frameBuildCommandList->BindIndexBuffer (*OSK::GRAPHICS::Sprite::globalIndexBuffer);

	frameBuildCommandList->BindMaterial(*renderer->GetFullscreenRenderingMaterial());
	frameBuildCommandList->BindMaterialSlot(*toneMappingPass->GetOutput().GetFullscreenSpriteMaterialSlot());
	frameBuildCommandList->DrawSingleInstance(6);

	frameBuildCommandList->BindMaterialSlot(*textRenderTarget->GetFullscreenSpriteMaterialSlot());
	frameBuildCommandList->DrawSingleInstance(6);

	frameBuildCommandList->BindMaterialSlot(*ecs->GetSystem<OSK::ECS::ColliderRenderSystem>()->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
	// frameBuildCommandList->DrawSingleInstance(6);

	frameBuildCommandList->BindMaterialSlot(*ecs->GetSystem<RayRenderSystem>()->GetRenderTarget().GetFullscreenSpriteMaterialSlot());
	frameBuildCommandList->DrawSingleInstance(6);

	frameBuildCommandList->EndGraphicsRenderpass();

	frameBuildCommandList->SetGpuImageBarrier(
		renderer->_GetSwapchain()->GetImage(resourceIndex),
		GpuImageLayout::COLOR_ATTACHMENT,
		GpuImageLayout::PRESENT,
		OSK::GRAPHICS::GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
		OSK::GRAPHICS::GpuBarrierInfo(GpuCommandStage::NONE,					GpuAccessStage::NONE)
	);
}

void Game::RegisterEcse() {
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	ecs->RegisterComponent<CarComponent>();
	ecs->RegisterComponent<EngineComponent>();
	ecs->RegisterComponent<CameraArmComponent>();
	ecs->RegisterComponent<CarAiComponent>();
	ecs->RegisterComponent<CircuitComponent>();
	ecs->RegisterComponent<CarControllerComponent>();

	ecs->RegisterEventType<CarInputEvent>();
	ecs->RegisterEventType<ExitEvent>();
	ecs->RegisterEventType<LoadLevelEvent>();
	ecs->RegisterEventType<PlayEvent>();
	ecs->RegisterEventType<SelectCarEvent>();
	ecs->RegisterEventType<RayCastEvent>();

	ecs->RegisterSystem<CarSystem>(-5);
	ecs->RegisterSystem<CarAiSystem>(-6);
	ecs->RegisterSystem<CarInputSystem>(-6);
	ecs->RegisterSystem<RayRenderSystem>(0)->Initialize(cameraObject, firstCar);

	ecs->RegisterSystem<CameraAttachmentSystem>(0);
	ecs->RegisterSystem<HubCameraSystem>(0);

	PauseSystems();
}

void Game::RegisterAssets() {
	OSK::Engine::GetAssetManager()->RegisterLoader<CarAssetsLoader>();
}

void Game::SetupUi() {
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	auto font = assetsManager->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json");
	font->LoadSizedFont(20);
	font->SetMaterial(material2d);

	OSK::OwnedPtr<OSK::UI::HorizontalContainer> logoContainer = new OSK::UI::HorizontalContainer({ 380.0f, 80.0f });

	auto buttonTexture = assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json");
	auto iconTexture = assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json");
	auto titleTexture = assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/GameTitle.json");

	const OSK::GRAPHICS::IGpuImageView* uiView = &buttonTexture->GetTextureView2D();
	const OSK::GRAPHICS::IGpuImageView* iconView = &iconTexture->GetTextureView2D();
	

	OSK::OwnedPtr<OSK::UI::ImageView> uiIcon = new OSK::UI::ImageView(OSK::Vector2f(48.0f));
	uiIcon->GetSprite().SetImageView(iconView);
	uiIcon->SetMargin(OSK::Vector4f(4.0f));

	OSK::OwnedPtr<OSK::UI::TextView> uiText = new OSK::UI::TextView(OSK::Vector2f(128.0f));
	uiText->SetFont(font);
	uiText->SetFontSize(35);
	uiText->SetText("OSKengine");
	uiText->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	uiText->AdjustSizeToText();

	gearText = new OSK::UI::TextView(OSK::Vector2f(148.0f));
	gearText->SetFont(font);
	gearText->SetFontSize(20);
	gearText->SetText("gear   ");
	gearText->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	gearText->AdjustSizeToText();

	rpmText = new OSK::UI::TextView(OSK::Vector2f(128.0f));
	rpmText->SetFont(font);
	rpmText->SetFontSize(20);
	rpmText->SetText("rpm    ");
	rpmText->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	rpmText->AdjustSizeToText();

	speedText = new OSK::UI::TextView(OSK::Vector2f(128.0f));
	speedText->SetFont(font);
	speedText->SetFontSize(20);
	speedText->SetText("rpm    ");
	speedText->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	speedText->AdjustSizeToText();

	logoContainer->GetSprite().SetImageView(uiView);
	logoContainer->GetSprite().color = OSK::Color(0.3f, 0.3f, 0.3f, 0.94f);

	logoContainer->SetPadding(OSK::Vector4f(4.0f));
	logoContainer->AddChild("icon", uiIcon.GetPointer());
	logoContainer->AddChild("text", uiText.GetPointer());

	logoContainer->AddChild("gearText", gearText);
	logoContainer->AddChild("rpmText", rpmText);
	logoContainer->AddChild("speedText", speedText);

	logoContainer->SetAnchor(OSK::UI::Anchor::TOP | OSK::UI::Anchor::LEFT);

	logoContainer->AdjustSizeToChildren();
	logoContainer->Rebuild();

	GetRootUiElement().SetKeepRelativeSize(true);
	GetRootUiElement().AddChild("", logoContainer.GetPointer());

	// Main menu
	auto mainMenu = new MainMenu(OSK::Engine::GetDisplay()->GetResolution().ToVector2f());

	auto onlineButton = UiBuilder::CreateMainMenuButton("MULTIPLAYER");
	onlineButton->GetDefaultSprite().SetImageView(uiView);
	onlineButton->GetDefaultSprite().color = OSK::Color(0.5f, 0.5f, 0.5f, 1.f);
	onlineButton->GetSelectedSprite().SetImageView(uiView);
	onlineButton->GetSelectedSprite().color = OSK::Color(0.5f, 0.5f, 0.5f, 1.f);
	onlineButton->Lock();
	// mainMenuButtons->AddChild("onlineButton", onlineButton.GetPointer());

	GetRootUiElement().AddChild(MainMenu::Name, mainMenu);


	// ------------ SELECTION --------------- //
	
}

void Game::ToMainMenu() {
	auto* mainMenu = static_cast<MainMenu*>(GetRootUiElement().GetChild(MainMenu::Name));

	mainMenu->SetVisible();
	mainMenu->ToMainMenu();

	PauseSystems();

	// Mouse input
	OSK::IO::IMouseInput* mouse = nullptr;
	OSK::Engine::GetInput()->QueryInterface(OSK::IUUID::IMouseInput, (void**)&mouse);
	mouse->SetReturnMode(OSK::IO::MouseReturnMode::FREE);
}

void Game::ToGame(std::string_view carAssetsPath) {
	m_hub.Unspawn();

	carSpawner.Spawn(OSK::Vector3f(-25.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(-15.f, 1.5f, -322.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(-7.f, 1.5f, -325.0f), carAssetsPath);
	firstCar = carSpawner.Spawn(OSK::Vector3f(0.0f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(7.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(18.f, 1.5f, -321.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(26.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(30.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(34.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(38.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(42.f, 1.5f, -320.0f), carAssetsPath);
	carSpawner.Spawn(OSK::Vector3f(46.f, 1.5f, -320.0f), carAssetsPath);

	circuit = circuitSpawner.Spawn();

	OSK::Engine::GetEcs()->GetSystem<CarAiSystem>()->SetCircuit(circuit);

	OSK::Engine::GetEcs()->GetSystem<RayRenderSystem>()->Initialize(cameraObject, firstCar);

	SetMainCar(firstCar);

	OSK::Engine::GetEcs()->GetSystem<HubCameraSystem>()->Deactivate();

	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->SetIbl(
		OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::IrradianceMap>("Resources/Assets/IBL/irradiance_circuit.json"),
		OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::SpecularMap>("Resources/Assets/IBL/specular_circuit.json")
	);

	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetIblConfig().irradianceStrength = 0.65f;
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetIblConfig().specularStrength = 0.65f;
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetIblConfig().radianceStrength = 1.0f;

	const auto newDirection = OSK::Vector3f(1.0f, -1.9f, 0.0f).GetNormalized();

	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.x = newDirection.x;
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.y = newDirection.y;
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetDirectionalLight().directionAndIntensity.Z = newDirection.z;

	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetShadowMap().SetSplits({ 20, 200, 500, 1000});
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetShadowMap().SetNearPlane(-15);
	OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>()->GetShadowMap().SetFarPlane(15);

	m_startNextFrame = true;

	// Mouse input
	OSK::IO::IMouseInput* mouse = nullptr;
	OSK::Engine::GetInput()->QueryInterface(OSK::IUUID::IMouseInput, (void**)&mouse);
	mouse->SetReturnMode(OSK::IO::MouseReturnMode::ALWAYS_RETURN);

	OSK::Engine::GetEcs()->GetComponent<EngineComponent>(firstCar).audioSource.Play();

	OSK::Engine::GetLogger()->InfoLog(std::format("VRAM: {} Mb", OSK::Engine::GetRenderer()->GetAllocator()->GetMemoryUsageInfo().gpuOnlyMemoryInfo.usedSpace / 1000000));
}

void Game::SetMainCar(OSK::ECS::GameObjectIndex obj) {
	auto* ecs = OSK::Engine::GetEcs();

	ecs->GetSystem<CarInputSystem>()->SetCar(obj);

	ecs->GetSystem<CameraAttachmentSystem>()->SetCar(obj);
	ecs->GetSystem<CameraAttachmentSystem>()->SetCamera(cameraArmObject);

	ecs->GetComponent<OSK::ECS::Transform3D>(cameraArmObject).AttachToObject(obj);
	ecs->GetComponent<OSK::ECS::Transform3D>(cameraArmObject).SetShouldInheritRotation(false);
}


void Game::PauseSystems() {
	auto* ecs = OSK::Engine::GetEcs();

	ecs->GetSystem<OSK::ECS::CollisionSystem>()->Deactivate();
	ecs->GetSystem<OSK::ECS::PhysicsResolver>()->Deactivate();
	ecs->GetSystem<OSK::ECS::PhysicsSystem>()->Deactivate();

	ecs->GetSystem<CarSystem>()->Deactivate();
	ecs->GetSystem<CarAiSystem>()->Deactivate();
	ecs->GetSystem<CarInputSystem>()->Deactivate();
	ecs->GetSystem<CameraAttachmentSystem>()->Deactivate();
}

void Game::UnpauseSystems() {
	auto* ecs = OSK::Engine::GetEcs();

	ecs->GetSystem<OSK::ECS::CollisionSystem>()->Activate();
	ecs->GetSystem<OSK::ECS::PhysicsResolver>()->Activate();
	ecs->GetSystem<OSK::ECS::PhysicsSystem>()->Activate();

	ecs->GetSystem<CarSystem>()->Activate();
	ecs->GetSystem<CarAiSystem>()->Activate();
	ecs->GetSystem<CarInputSystem>()->Activate();
	ecs->GetSystem<CameraAttachmentSystem>()->Activate();
}

void Game::SpanwCamera() {
	auto* ecs = OSK::Engine::GetEcs();

	cameraObject = ecs->SpawnObject();
	cameraArmObject = ecs->SpawnObject();

	ecs->AddComponent<OSK::ECS::Transform3D>(cameraArmObject, OSK::ECS::Transform3D(cameraArmObject));
	ecs->AddComponent<CameraArmComponent>(cameraArmObject, {});

	auto* cameraTransform = &ecs->AddComponent<OSK::ECS::Transform3D>(cameraObject, OSK::ECS::Transform3D(cameraObject));
	cameraTransform->AddPosition({ 0.0f, 0.1f, -4.1f }); // { 0.0f, 0.1f, -4.1f }
	ecs->AddComponent<OSK::ECS::CameraComponent3D>(cameraObject, {});

	cameraTransform->AttachToObject(cameraArmObject);
}

void Game::SetupTreeNormals() {
	auto* renderSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::DeferredRenderSystem>();
	auto* treeRenderSystem = OSK::Engine::GetEcs()->GetSystem<OSK::ECS::TreeNormalsRenderSystem>();

	std::array<const OSK::GRAPHICS::IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> normalsImages{};
	const auto viewConfig = OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		normalsImages[i] = treeRenderSystem->GetRenderTarget().GetMainColorImage(i)->GetView(viewConfig);

	auto* treeGBufferPass = renderSystem->GetRenderPass(OSK::GRAPHICS::TreeGBufferPass::GetRenderPassName())->As<OSK::GRAPHICS::TreeGBufferPass>();
	treeGBufferPass->GetMaterialInstance()->GetSlot("normals")->SetGpuImages("preCalculatedNormalTexture", normalsImages);
	treeGBufferPass->GetMaterialInstance()->GetSlot("normals")->FlushUpdate();
}
