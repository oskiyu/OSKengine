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
	cameraTransform.SetPosition({ 0, 0.5f, -1.0f });
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

	toneMappingPass->SetInput(bloomPass->GetOutput(), OSK::GRAPHICS::GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	toneMappingPass->UpdateMaterialInstance();

	RegisterEcse();
	SetupUi();

	carSpawner.SetMaterial3D(material3d);
	const OSK::ECS::GameObjectIndex carObject = carSpawner.Spawn();

	circuitSpawner.SetMaterial3D(material3d);
	circuitSpawner.Spawn();

	ecs->GetComponent<OSK::ECS::Transform3D>(cameraObject)
		.AttachToObject(carObject);

	ecs->GetSystem<CarInputSystem>()->SetCar(carObject);
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
		this->Exit();

	ecs->GetComponent<OSK::ECS::CameraComponent3D>(cameraObject).UpdateTransform(
		&ecs->GetComponent<OSK::ECS::Transform3D>(cameraObject)
	);
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
	OSK::GRAPHICS::ICommandList* computeCommandList = renderer->GetPostComputeCommandList();
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
}

void Game::SetupUi() {
	OSK::ASSETS::AssetManager* assetsManager = OSK::Engine::GetAssetManager();

	OSK::ASSETS::Font* font = assetsManager->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font0.json", "GLOBAL");
	font->LoadSizedFont(22);
	font->SetMaterial(material2d);

	OSK::OwnedPtr<OSK::UI::HorizontalContainer> logoContainer = new OSK::UI::HorizontalContainer({ 380.0f, 80.0f });

	const OSK::GRAPHICS::IGpuImageView* uiView = 
		&assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/button_texture.json", "GLOBAL")
		->GetTextureView2D();
	const OSK::GRAPHICS::IGpuImageView* iconView = 
		&assetsManager->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/engine_icon.json", "GLOBAL")
		->GetTextureView2D();

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

	GetRootUiElement().AddChild("", logoContainer.GetPointer());

	// Main menu
	OSK::OwnedPtr<OSK::UI::VerticalContainer> mainMenu = new OSK::UI::VerticalContainer(OSK::Engine::GetDisplay()->GetResolution().ToVector2f() * 0.6f);
	mainMenu->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	mainMenu->SetMargin(OSK::Vector2f(300.0f, 200.0f));

	OSK::OwnedPtr<OSK::UI::VerticalContainer> mainMenuButtons = new OSK::UI::VerticalContainer(mainMenu->GetContentSize());
	mainMenuButtons->SetAnchor(OSK::UI::Anchor::CENTER_X | OSK::UI::Anchor::CENTER_Y);
	mainMenuButtons->GetSprite().SetImageView(uiView);
	mainMenuButtons->GetSprite().color = OSK::Color(0.3f, 0.3f, 0.3f, 0.7f);
	mainMenu->AddChild("buttons", mainMenuButtons.GetPointer());
	
	OSK::OwnedPtr<OSK::UI::Button> playButton = new OSK::UI::Button(OSK::Vector2f(100.0f, 80.0f), "PLAY");
	playButton->SetTextFont(font);
	playButton->SetAnchor(OSK::UI::Anchor::RIGHT | OSK::UI::Anchor::BOTTOM);
	mainMenuButtons->AddChild("playButton", playButton.GetPointer());

	mainMenuButtons->Rebuild();
	mainMenu->Rebuild();

	GetRootUiElement().AddChild("mainMenu", mainMenu.GetPointer());
}
