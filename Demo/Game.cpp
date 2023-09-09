#include "Game.h"

#include <OSKengine/OSKengine.h>

#include <OSKengine/IKeyboardInput.h>
#include <OSKengine/IGamepadInput.h>
#include <OSKengine/IMouseInput.h>
#include <OSKengine/MouseModes.h>

#include <OSKengine/IDisplay.h>
#include <OSKengine/Viewport.h>
#include <OSKengine/RenderSystem3D.h>
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

#include <OSKengine/AssetManager.h>
#include <OSKengine/IrradianceMap.h>
#include <OSKengine/SpecularMap.h>
#include <OSKengine/CubemapTexture.h>
#include <OSKengine/Texture.h>
#include <OSKengine/Font.h>

#include <OSKengine/IUiContainer.h>
#include <OSKengine/UiHorizontalContainer.h>
#include <OSKengine/UiVerticalContainer.h>
#include <OSKengine/UiImageView.h>
#include <OSKengine/UiTextView.h>
#include <OSKengine/UiButton.h>

#include "CarComponent.h"
#include "CarInputEvent.h"
#include "CarInputSystem.h"
#include "CarSystem.h"

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

	cameraObject = ecs->SpawnObject();

	OSK::ECS::Transform3D cameraTransform(cameraObject);
	cameraTransform.SetPosition(OSK::Vector3f(0.0f, 0.5f, -1.0f) * 1.2f);
	cameraTransform.RotateWorldSpace(glm::radians(10.0f), { 1.0f, 0.0f, 0.0f });

	ecs->AddComponent(cameraObject, OSK::ECS::CameraComponent3D());
	ecs->AddComponent(cameraObject, cameraTransform);

	ecs->GetSystem<OSK::ECS::RenderSystem3D>()->Initialize(
		cameraObject,
		*assetsManager->Load<OSK::ASSETS::IrradianceMap>("Resources/Assets/IBL/specular0.json", "GLOBAL"),
		*assetsManager->Load<OSK::ASSETS::SpecularMap>("Resources/Assets/IBL/irradiance0.json", "GLOBAL")
	);

	ecs->GetSystem<OSK::ECS::SkyboxRenderSystem>()->SetCamera(cameraObject);
	ecs->GetSystem<OSK::ECS::SkyboxRenderSystem>()->SetCubemap(
		*assetsManager->Load<OSK::ASSETS::CubemapTexture>("Resources/Assets/Skyboxes/skybox0.json", "GLOBAL")
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

	bloomPass = new OSK::GRAPHICS::BloomPass();
	bloomPass->Create(OSK::Engine::GetDisplay()->GetResolution());

	bloomPass->SetInput(finalFrameCombiner->GetRenderTarget(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	bloomPass->UpdateMaterialInstance();

	toneMappingPass = new OSK::GRAPHICS::ToneMappingPass();
	toneMappingPass->Create(OSK::Engine::GetDisplay()->GetResolution());
	toneMappingPass->SetExposure(7.0f);

	toneMappingPass->SetInput(bloomPass->GetOutput(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	toneMappingPass->UpdateMaterialInstance();

	RegisterEcse();
	SetupUi();

	carSpawner.SetMaterial3D(material3d);
	firstCar = carSpawner.Spawn();
	secondCar = carSpawner.Spawn();
	carSpawner.Spawn();

	circuitSpawner.SetMaterial3D(material3d);
	circuitSpawner.Spawn();

	ecs->GetComponent<OSK::ECS::Transform3D>(cameraObject)
		.AttachToObject(firstCar);

	ecs->GetSystem<CarInputSystem>()->SetCar(firstCar);
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

	if (keyboard->IsKeyDown(OSK::IO::Key::ESCAPE))
		ToMainMenu();

	if (keyboard->IsKeyStroked(OSK::IO::Key::Q)) {
		collisionTesting.Start(firstCar, secondCar);
	}

	if (mouse) {
		GetRootUiElement().UpdateByCursor(
			mouse->GetMouseState().GetPosition().ToVector2f(),
			mouse->GetMouseState().IsButtonDown(OSK::IO::MouseButton::BUTTON_LEFT),
			OSK::Vector2f::Zero);
	}

	ecs->GetComponent<OSK::ECS::CameraComponent3D>(cameraObject).UpdateTransform(
		&ecs->GetComponent<OSK::ECS::Transform3D>(cameraObject)
	);

	collisionTesting.Update();
}

void Game::OnExit() {
	finalFrameCombiner.Delete();
	textRenderTarget.Delete();
}

void Game::OnWindowResize(const OSK::Vector2ui& newRes) {
	if (finalFrameCombiner.HasValue())
		finalFrameCombiner->Resize(newRes);
	if (textRenderTarget.HasValue())
		textRenderTarget->Resize(newRes);

	bloomPass->Resize(newRes);
	toneMappingPass->Resize(newRes);

	const OSK::GRAPHICS::GpuImageViewConfig viewConfig
		= OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	bloomPass->SetInput(finalFrameCombiner->GetRenderTarget(), viewConfig);
	toneMappingPass->SetInput(bloomPass->GetOutput(), viewConfig);

	bloomPass->UpdateMaterialInstance();
	toneMappingPass->UpdateMaterialInstance();
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
		= ecs->GetSystem<OSK::ECS::RenderSystem3D>()->GetRenderTarget().GetMainColorImage(resourceIndex);

	frameBuildCommandList->SetGpuImageBarrier(skyboxRenderSystemImg,
		GpuImageLayout::SAMPLED,
		OSK::GRAPHICS::GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	finalFrameCombiner->Begin(frameBuildCommandList, OSK::GRAPHICS::FrameCombiner::ImageFormat::RGBA16);
	finalFrameCombiner->Draw(frameBuildCommandList, *skyboxRenderSystemImg->GetView(OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	finalFrameCombiner->Draw(frameBuildCommandList, *sceneRenderSystemImg->GetView(OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0)));
	finalFrameCombiner->End(frameBuildCommandList);

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
	ecs->RegisterEventType<CarInputEvent>();
	ecs->RegisterSystem<CarSystem>(-5);
	CarInputSystem* inputSystem = ecs->RegisterSystem<CarInputSystem>(-5);
	// inputSystem->SetCar(car)

	ecs->RegisterSystem<OSK::ECS::ColliderRenderSystem>(0)->Initialize(cameraObject);

	PauseSystems();
}


void Game::SetupUi() {
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	OSK::ASSETS::Font* font = assetsManager->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json", "GLOBAL");
	font->LoadSizedFont(20);
	font->SetMaterial(material2d);

	OSK::OwnedPtr<OSK::UI::HorizontalContainer> logoContainer = new OSK::UI::HorizontalContainer({ 380.0f, 80.0f });

	const OSK::GRAPHICS::IGpuImageView* uiView = 
		&assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json", "GLOBAL")
		->GetTextureView2D();
	const OSK::GRAPHICS::IGpuImageView* iconView = 
		&assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json", "GLOBAL")
		->GetTextureView2D();

	const OSK::Vector2f buttonSize = OSK::Vector2f(180.0f, 50.0f);

	const OSK::Color buttonColor = OSK::Color(0.3f, 0.36f, 1.0f, 1.0f);
	const OSK::Color buttonSelectedColor = OSK::Color(0.3f, 0.5f, 1.0f, 1.0f);
	const OSK::Color buttonPressedColor = OSK::Color(0.5f, 0.7f, 1.0f, 1.0f);

	OSK::Color panelColor = OSK::Color(0.4f, 0.45f, 0.7f, 0.98f);

	OSK::OwnedPtr<OSK::UI::ImageView> uiIcon = new OSK::UI::ImageView(OSK::Vector2f(48.0f));
	uiIcon->GetSprite().SetImageView(iconView);
	uiIcon->SetMargin(OSK::Vector4f(4.0f));

	OSK::OwnedPtr<OSK::UI::TextView> uiText = new OSK::UI::TextView(OSK::Vector2f(128.0f));
	uiText->SetFont(font);
	uiText->SetFontSize(35);
	uiText->SetText("OSKengine");
	uiText->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	uiText->AdjustSizeToText();

	logoContainer->GetSprite().SetImageView(uiView);
	logoContainer->GetSprite().color = OSK::Color(0.3f, 0.3f, 0.3f, 0.94f);

	logoContainer->SetPadding(OSK::Vector4f(4.0f));
	logoContainer->AddChild("icon", uiIcon.GetPointer());
	logoContainer->AddChild("text", uiText.GetPointer());

	logoContainer->SetAnchor(OSK::UI::Anchor::TOP | OSK::UI::Anchor::LEFT);

	logoContainer->AdjustSizeToChildren();
	logoContainer->Rebuild();

	GetRootUiElement().SetKeepRelativeSize(true);
	GetRootUiElement().AddChild("", logoContainer.GetPointer());

	// Main menu
	OSK::OwnedPtr<OSK::UI::FreeContainer> mainMenu = new OSK::UI::FreeContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f());
	mainMenu->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	mainMenu->SetMargin(OSK::Vector2f(300.0f, 200.0f));
	mainMenu->SetKeepRelativeSize(true);

	OSK::OwnedPtr<OSK::UI::FreeContainer> menuLine = new OSK::UI::FreeContainer({ 2000.0f, 85.0f });
	menuLine->SetKeepRelativeSize(true);
	menuLine->GetSprite().SetImageView(uiView);
	menuLine->GetSprite().color = OSK::Color(0.3f, 0.3f, 0.9f, 0.8f);
	menuLine->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::BOTTOM);
	menuLine->SetMargin(OSK::Vector2f(0.0f, 550.0f));
	mainMenu->AddChild("line", menuLine.GetPointer());

	OSK::OwnedPtr<OSK::UI::HorizontalContainer> mainMenuButtons = new OSK::UI::HorizontalContainer({ mainMenu->GetContentSize().x, 20.0f });
	mainMenuButtons->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	mainMenuButtons->SetKeepRelativeSize(true);
	menuLine->AddChild("buttons", mainMenuButtons.GetPointer());

	std::function<void(OSK::UI::Button*)> SetupButton = [font, uiView, buttonColor, buttonSelectedColor, buttonPressedColor](OSK::UI::Button* button) {
		button->SetKeepRelativeSize(true);
		button->SetTextFont(font);
		button->SetTextFontSize(32);
		button->SetType(OSK::UI::Button::Type::NORMAL);
		button->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
		button->GetDefaultSprite().SetImageView(uiView);
		button->GetDefaultSprite().color = buttonColor;
		button->GetSelectedSprite().SetImageView(uiView);
		button->GetSelectedSprite().color = buttonSelectedColor;
		button->GetPressedSprite().SetImageView(uiView);
		button->GetPressedSprite().color = buttonPressedColor;
		};

	std::function<void(OSK::UI::FreeContainer*)> SetupPanel = [uiView, panelColor] (OSK::UI::FreeContainer* panel) {
		panel->SetKeepRelativeSize(true);
		panel->GetSprite().SetImageView(uiView);
		panel->GetSprite().color = panelColor;
	};

	OSK::OwnedPtr<OSK::UI::Button> playButton = new OSK::UI::Button(buttonSize, "PLAY");
	SetupButton(playButton.GetPointer());
	playButton->SetCallback([&](bool) {
		GetRootUiElement().GetChild("mainMenu")->SetInvisible();
		GetRootUiElement().GetChild("title")->SetInvisible();
		UnpauseSystems();
		});
	mainMenuButtons->AddChild("playButton", playButton.GetPointer());

	OSK::OwnedPtr<OSK::UI::Button> aboutButton = new OSK::UI::Button(buttonSize, "ABOUT");
	SetupButton(aboutButton.GetPointer());
	aboutButton->SetText("ABOUT");
	aboutButton->SetCallback([&](bool) {
		GetRootUiElement().GetChild("mainMenu")->SetInvisible();
		GetRootUiElement().GetChild("title")->SetInvisible();
		GetRootUiElement().GetChild("about")->SetVisible();
		});
	mainMenuButtons->AddChild("aboutButton", aboutButton.GetPointer());

	OSK::OwnedPtr<OSK::UI::Button> optionsButton = new OSK::UI::Button(buttonSize, "AJUSTES");
	SetupButton(optionsButton.GetPointer());
	optionsButton->SetText("AJUSTES");
	optionsButton->SetCallback([&](bool) {
		GetRootUiElement().GetChild("mainMenu")->SetInvisible();
		GetRootUiElement().GetChild("title")->SetInvisible();
		GetRootUiElement().GetChild("options")->SetVisible();
		});
	mainMenuButtons->AddChild("optionsButton", optionsButton.GetPointer());

	OSK::OwnedPtr<OSK::UI::Button> onlineButton = new OSK::UI::Button(buttonSize, "MULTIPLAYER");
	SetupButton(onlineButton.GetPointer());
	onlineButton->SetText("MULTIJUGADOR");
	onlineButton->GetDefaultSprite().SetImageView(uiView);
	onlineButton->GetDefaultSprite().color = OSK::Color(0.5f, 0.5f, 0.5f, 1.f);
	onlineButton->GetSelectedSprite().SetImageView(uiView);
	onlineButton->GetSelectedSprite().color = OSK::Color(0.5f, 0.5f, 0.5f, 1.f);
	onlineButton->Lock();
	mainMenuButtons->AddChild("onlineButton", onlineButton.GetPointer());

	OSK::OwnedPtr<OSK::UI::Button> exitButton = new OSK::UI::Button(buttonSize, "EXIT");
	SetupButton(exitButton.GetPointer());
	exitButton->SetText("EXIT");
	exitButton->SetCallback([&](bool) { this->Exit(); });
	mainMenuButtons->AddChild("exitButton", exitButton.GetPointer());

	// TITLE
	const OSK::GRAPHICS::IGpuImageView* titleView =
		&assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/GameTitle.json", "GLOBAL")
		->GetTextureView2D();

	OSK::OwnedPtr<OSK::UI::FreeContainer> titleContainer = new OSK::UI::FreeContainer({ 1200.0f, 600.0f });
	titleContainer->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	OSK::OwnedPtr<OSK::UI::ImageView> uiTitle = new OSK::UI::ImageView(titleView->GetSize2D().ToVector2f() * 0.4f);
	uiTitle->SetKeepRelativeSize(true);
	uiTitle->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);
	uiTitle->GetSprite().SetImageView(titleView);
	uiTitle->SetMargin(OSK::Vector4f(25.0f));

	titleContainer->AddChild("title", uiTitle.GetPointer());
	titleContainer->SetKeepRelativeSize(true);
	GetRootUiElement().AddChild("title", titleContainer.GetPointer());

	mainMenuButtons->AdjustSizeToChildren();
	mainMenuButtons->Rebuild();
	menuLine->Rebuild();
	mainMenu->Rebuild();

	// About

	auto aboutPanel = new OSK::UI::FreeContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.8f);
	SetupPanel(aboutPanel);

	auto aboutContent = new OSK::UI::VerticalContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.6f);

	aboutPanel->AddChild("content", aboutContent);

	auto aboutTitle = new OSK::UI::TextView(OSK::Vector2f(100.0f, 50.0f));
	aboutTitle->SetFont(font);
	aboutTitle->SetFontSize(32);
	aboutTitle->SetText("About");
	aboutTitle->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	aboutPanel->AddChild("title", aboutTitle);

	auto aboutBackButton = new OSK::UI::Button({ 100.0f, 50.0f });
	SetupButton(aboutBackButton);
	aboutBackButton->SetText("Back");
	aboutBackButton->SetAnchor(OSK::UI::Anchor::LEFT |OSK::UI::Anchor::BOTTOM);
	aboutBackButton->SetCallback([&](bool) { ToMainMenu(); });

	auto about1 = new OSK::UI::TextView({ 400.0f, 50.0f });
	about1->SetFont(font);
	about1->SetFontSize(20);
	about1->SetText(std::format("OSKengine by oskiyu\n\tBuild {}\n\t(c) 2019 - 2023\n\n{}", OSK::Engine::GetBuild(),
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"));
	// about1->SetAnchor(OSK::UI::Anchor::LEFT);
	about1->AdjustSizeToText();

	aboutContent->AddChild("1", about1);

	aboutPanel->AddChild("back", aboutBackButton);

	aboutPanel->SetInvisible();
	GetRootUiElement().AddChild("about", aboutPanel);

	// Options

	auto optionsPanel = new OSK::UI::FreeContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.8f);
	SetupPanel(optionsPanel);

	auto optionsContent = new OSK::UI::VerticalContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.6f);

	optionsPanel->AddChild("content", optionsContent);

	auto optionsTitle = new OSK::UI::TextView(OSK::Vector2f(100.0f, 50.0f));
	optionsTitle->SetFont(font);
	optionsTitle->SetFontSize(35);
	optionsTitle->SetText("Ajustes");
	optionsTitle->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::TOP);

	optionsPanel->AddChild("title", optionsTitle);
	optionsPanel->SetKeepRelativeSize(true);

	auto optionsBackButton = new OSK::UI::Button({ 100.0f, 50.0f });
	SetupButton(optionsBackButton);
	optionsBackButton->SetText("Back");
	optionsBackButton->SetCallback([&](bool) { ToMainMenu(); });
	optionsBackButton->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::BOTTOM);

	auto options1 = new OSK::UI::TextView({ 400.0f, 50.0f });
	options1->SetFont(font);
	options1->SetFontSize(25);
	options1->SetText("OSKengine by oskiyu\n\t(c) 2019 - 2023");
	options1->SetAnchor(OSK::UI::Anchor::LEFT);
	options1->AdjustSizeToText();

	optionsContent->AddChild("1", options1);

	optionsPanel->AddChild("back", optionsBackButton);

	optionsPanel->SetInvisible();
	GetRootUiElement().AddChild("options", optionsPanel);

	GetRootUiElement().AddChild("mainMenu", mainMenu.GetPointer());
}

void Game::ToMainMenu() {
	GetRootUiElement().GetChild("mainMenu")->SetVisible();
	GetRootUiElement().GetChild("title")->SetVisible();

	// Resetar cualquier otro menú.
	GetRootUiElement().GetChild("about")->SetInvisible();
	GetRootUiElement().GetChild("options")->SetInvisible();

	PauseSystems();
}

void Game::PauseSystems() {
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	ecs->GetSystem<OSK::ECS::CollisionSystem>()->Deactivate();
	ecs->GetSystem<OSK::ECS::PhysicsResolver>()->Deactivate();
	ecs->GetSystem<OSK::ECS::PhysicsSystem>()->Deactivate();
	ecs->GetSystem<CarSystem>()->Deactivate();
	ecs->GetSystem<CarInputSystem>()->Deactivate();
}

void Game::UnpauseSystems() {
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	ecs->GetSystem<OSK::ECS::CollisionSystem>()->Activate();
	ecs->GetSystem<OSK::ECS::PhysicsResolver>()->Activate();
	ecs->GetSystem<OSK::ECS::PhysicsSystem>()->Activate();
	ecs->GetSystem<CarSystem>()->Activate();
	ecs->GetSystem<CarInputSystem>()->Activate();
}
