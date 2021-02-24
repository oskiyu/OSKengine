#include "Game.h"

#include "Components.h"

void Game::SetupWindow() {
	window = new OSK::WindowAPI();
	window->SetWindow(WindowCreateInfo.SizeX, WindowCreateInfo.SizeY, WindowCreateInfo.Name);
	window->SetMouseMovementMode(WindowCreateInfo.MouseAcceleration);
	window->SetMouseMode(WindowCreateInfo.MouseMode);
}

void Game::SetupRenderer() {
	renderer = new OSK::RenderAPI();

	renderer->Window = window;
	renderer->SetPresentMode(RendererCreateInfo.VSyncMode);
	renderer->FPSlimit = RendererCreateInfo.FPSlimit;
	renderer->Init(RendererCreateInfo.GameName, RendererCreateInfo.GameVersion);
}

void Game::SetupIU() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), true);

	renderer->ImGuiDescriptorLayout = renderer->CreateNewDescriptorLayout();
	renderer->ImGuiDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_SAMPLER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(8, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(9, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000);
	renderer->ImGuiDescriptorLayout->AddBinding(10, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000);
	renderer->ImGuiDescriptorLayout->Create(1000);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = renderer->Instance;
	init_info.PhysicalDevice = renderer->GPU;
	init_info.Device = renderer->LogicalDevice;
	OSK::VULKAN::QueueFamilyIndices indices = renderer->findQueueFamilies(renderer->GPU);
	init_info.QueueFamily = indices.GraphicsFamily.value();
	init_info.Queue = renderer->GraphicsQ;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = renderer->ImGuiDescriptorLayout->VulkanDescriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = renderer->SwapchainImages.size();
	init_info.ImageCount = renderer->SwapchainImages.size();
	init_info.CheckVkResultFn = nullptr;

	renderer->ImGuiRenderpass = renderer->CreateNewRenderpass();

	OSK::VULKAN::RenderpassAttachment attachment;
	attachment.AddAttachment(renderer->SwapchainFormat, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	attachment.attahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	OSK::VULKAN::RenderpassSubpass subpass;
	OSK::VULKAN::SubpassDependency dependency;
	dependency.VulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.VulkanDependency.dstSubpass = 0;
	dependency.VulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.VulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.VulkanDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.VulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	subpass.AddDependency(dependency);
	subpass.SetColorAttachments({ attachment });
	subpass.SetPipelineBindPoint();
	subpass.Set();

	renderer->ImGuiRenderpass->AddAttachment(attachment);
	renderer->ImGuiRenderpass->AddSubpass(subpass);
	renderer->ImGuiRenderpass->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
	renderer->ImGuiRenderpass->Create();

	ImGui_ImplVulkan_Init(&init_info, renderer->ImGuiRenderpass->VulkanRenderpass);

	//
	VkCommandBuffer cmdBuffer = renderer->beginSingleTimeCommandBuffer();
	ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);
	renderer->endSingleTimeCommandBuffer(cmdBuffer);
}

void Game::Run() {
	OnCreate();

	Init();

	MainLoop();

	OnExit();

	Close();
}

OSK::RenderAPI* Game::GetRenderer() {
	return renderer;
}
OSK::WindowAPI* Game::GetWindow() {
	return window;
}
OSK::AudioSystem* Game::GetAudioSystem() {
	return audio;
}
deltaTime_t Game::GetFPS() {
	return Framerate;
}

void Game::Init() {
	SetupWindow();
	SetupRenderer();

	Content = renderer->Content;

	SetupSystems();
	SetupIU();

	LoadContent();

	PhysicsSystem->FloorTerrain = RenderSystem3D->RScene->Terreno;

	renderer->OSKengineIconSprite.SpriteTransform.SetPosition({ 5.0f });
	renderer->OSKengineIconSprite.SpriteTransform.SetScale({ 48.f });
}

void Game::SetupSystems() {
	ECS = new OSK::EntityComponentSystem();

	ECS->RegisterComponent<OSK::PhysicsComponent>();
	ECS->RegisterComponent<OSK::InputComponent>();
	ECS->RegisterComponent<OSK::ModelComponent>();
	ECS->RegisterComponent<OSK::OnTickComponent>();
	ECS->RegisterComponent<OSK::CollisionComponent>();

	PhysicsSystem = ECS->RegisterSystem<OSK::PhysicsScene>();
	InputSystem = ECS->RegisterSystem<OSK::InputSystem>();
	RenderSystem3D = ECS->RegisterSystem<OSK::RenderSystem3D>();
	OnTickSystem = ECS->RegisterSystem<OSK::OnTickSystem>();

	InputSystem->SetWindow(window);
	RenderSystem3D->Renderer = renderer;
	RenderSystem3D->Init();
}

void Game::Close() {
	delete renderer;
	delete window;
	delete audio;

	delete ECS;
}

void Game::Exit() {
	window->Close();
}

void Game::MainLoop() {
	deltaTime_t deltaTime = 1.0f;

	while (!window->WindowShouldClose()) {
		deltaTime_t startTime = window->GetTime();
		window->PollEvents();
		window->UpdateKeyboardState(NewKS);
		window->UpdateMouseState(NewMS);

		//Update
		ECS->OnTick(deltaTime);

		OnTick(deltaTime);

		//FPS
		FramerateDeltaTime += deltaTime;
		FramerateCount++;
		if (FramerateDeltaTime > 1.0f) {
			Framerate = FramerateCount;
			FramerateCount = 0;
			FramerateDeltaTime = 0.0f;
		}

		//Draw
		renderer->DefaultCamera3D.updateVectors();

		OnDraw2D();

		renderer->RenderFrame();

		//End
		OldKS = NewKS;
		OldMS = NewMS;

		deltaTime_t endTime = window->GetTime();
		deltaTime = endTime - startTime;
	}

	Close();
}