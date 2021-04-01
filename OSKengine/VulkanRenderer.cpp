#include "VulkanRenderer.h"

#include "Log.h"
#include "OSKmacros.h"
#include "OSKsettings.h"
#include <vector>

#include "GPUinfo.h"
#include "PresentMode.h"
#include <set>
#include <algorithm>
#include <array>

#include "Vertex.h"
#include "FileIO.h"
#include "stbi_image.h"
#include "Sprite.h"
#include "SpriteBatch.h"

#include "WindowAPI.h"
#include <gtc/type_ptr.hpp>
#include "Model.h"
#include "RenderSystem3D.h"

#include <ft2build.h>
#include <thread>
#include "Camera3D.h"
#include FT_FREETYPE_H

using namespace OSK;
using namespace OSK::VULKAN;

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;


void RenderAPI::SetViewport(VkCommandBuffer& cmdBuffer, int32_t x, int32_t y, uint32_t sizeX, uint32_t sizeY) const {
	VkViewport viewport{};
	viewport.x = x;
	viewport.y = y;
	if (sizeX != 0)
		viewport.width = sizeX;
	else
		viewport.width = Window->ScreenSizeX;
	if (sizeY != 0)
		viewport.height = sizeY;
	else
		viewport.height = Window->ScreenSizeY;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	VkRect2D scissor{};
	scissor.extent = { (uint32_t)viewport.width, (uint32_t)viewport.height };
	scissor.offset = { 0, 0 };
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
}

VkPresentModeKHR translatePresentMode(const PresentMode& mode) {
	switch (mode) {
		case PresentMode::INMEDIATE:
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case PresentMode::VSYNC:
			return VK_PRESENT_MODE_FIFO_KHR;
		case PresentMode::VSYNC_TRIPLE_BUFFER:
			return VK_PRESENT_MODE_MAILBOX_KHR;

		default:
			return VK_PRESENT_MODE_FIFO_KHR;
	}
}


//Extensiones de la GPU que van a ser necesarias.
const std::vector<const char*> gpuExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


//Destruye la consola de capas devalidación.
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}


//Comprueba si la GPU soporta estas extensiones.
bool checkGPUextensionSupport(VkPhysicalDevice gpu) {
	//Obtener el número de extensiones.
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);

	//Obtener las extensiones.
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, availableExtensions.data());

	//Nombre de las extensiones.
	std::set<std::string> requiredExtensions(gpuExtensions.begin(), gpuExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}


//Obtiene el formato que se va a usar.
VkSurfaceFormatKHR getSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	//Preferir sRGB.
	for (const auto& format : formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format;

	return formats[0];
}

OskResult RenderAPI::Init(const std::string& appName, const Version& gameVersion) {
	VulkanImageGen::SetRenderer(this);

	createInstance(appName, gameVersion);

	if (checkValidationLayers())
		setupDebugConsole();

	createSurface();
	getGPU();
	createLogicalDevice();
	createSwapchain();
	createSwapchainImageViews();

	MSystem = new MaterialSystem(this);

	createRenderpass();
	
	RenderTargetSizeX = SwapchainExtent.width * RenderResolutionMultiplier;
	RenderTargetSizeY = SwapchainExtent.height * RenderResolutionMultiplier;

	{
		RenderpassAttachment clrAttachment{};
		clrAttachment.AddAttachment(SwapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);//
		clrAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		RenderpassAttachment dpthAttachment{};
		dpthAttachment.AddAttachment(getDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		dpthAttachment.CreateReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		std::vector<RenderpassAttachment> attchments = { clrAttachment };

		RTarget->CreateRenderpass(attchments, &dpthAttachment);
	}

	createCommandPool();
	createGlobalImageSampler();
	createDefaultUniformBuffers();
	ContentManager::DefaultTexture = Content->LoadTexture(ContentManager::DEFAULT_TEXTURE_PATH);
	Material::DefaultTexture = ContentManager::DefaultTexture;

	//MATERIALS
	{
		MSystem->RegisterMaterial(DefaultMaterial2D_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture" });
		MSystem->GetMaterial(DefaultMaterial2D_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.CullMode = PolygonCullMode::NONE;
		pipelineInfo.VertexPath = Settings.VertexShaderPath2D;
		pipelineInfo.FragmentPath = Settings.FragmentShaderPath2D;
		pipelineInfo.PushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst2D) });
		MSystem->GetMaterial(DefaultMaterial2D_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		MSystem->RegisterMaterial(DefaultMaterial3D_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Animation" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Model" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Albedo" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::FRAGMENT, "Lights" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Specular" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Shadows" });
		MSystem->GetMaterial(DefaultMaterial3D_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.VertexPath = Settings.VertexShaderPath3D;
		pipelineInfo.FragmentPath = Settings.FragmentShaderPath3D;
		pipelineInfo.UseDepthStencil = true;
		pipelineInfo.PushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst3D) });
		MSystem->GetMaterial(DefaultMaterial3D_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		MSystem->RegisterMaterial(DefaultSkyboxMaterial_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture" });
		MSystem->GetMaterial(DefaultSkyboxMaterial_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.CullMode = PolygonCullMode::FRONT;
		pipelineInfo.FrontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;
		pipelineInfo.VertexPath = Settings.SkyboxVertexPath;
		pipelineInfo.FragmentPath = Settings.SkyboxFragmentPath;
		pipelineInfo.UseDepthStencil = false;
		MSystem->GetMaterial(DefaultSkyboxMaterial_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		MSystem->RegisterMaterial(DefaultShadowsMaterial_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Animation" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Model" });
		MSystem->GetMaterial(DefaultShadowsMaterial_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.PMode = PolygonMode::FILL;
		pipelineInfo.CullMode = PolygonCullMode::FRONT;
		pipelineInfo.FrontFaceType = PolygonFrontFaceType::CLOCKWISE;
		pipelineInfo.VertexPath = "shaders/VK_Shadows/vert.spv";
		pipelineInfo.FragmentPath = "shaders/VK_Shadows/frag.spv";
		pipelineInfo.UseDepthStencil = true;
		MSystem->GetMaterial(DefaultShadowsMaterial_Name)->SetPipelineSettings(pipelineInfo);
	}
	//!MATERIALS

	createDepthResources();
	createFramebuffers();
	createSyncObjects();
	createCommandBuffers();

	createSpriteIndexBuffer();

	Skybox::Model = Content->LoadModelData("models/Skybox/cube.obj");

	DefaultCamera2D = Camera2D(Window);
	DefaultCamera3D.Window = Window;

	hasBeenInit = true;

	Content->LoadSprite(OSKengineIconSprite, "Resources/OSKengine_icon_lowres.png");
	Content->LoadSprite(OSK_IconSprite, "Resources/OSK_icon_lowres.png");

	RTarget->CreateSprite(Content);

	//Image
	RTarget->Size.X = RenderTargetSizeX;
	RTarget->Size.Y = RenderTargetSizeY;
	VULKAN::VulkanImageGen::CreateImage(&RTarget->RenderedSprite.Texture2D->Image, RTarget->Size, SwapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&RTarget->RenderedSprite.Texture2D->Image, SwapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(RTarget->RenderedSprite.Texture2D->Image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	RTarget->RenderedSprite.SpriteMaterial->SetTexture(RTarget->RenderedSprite.Texture2D);
	RTarget->RenderedSprite.SpriteMaterial->FlushUpdate();
	
	createRenderTarget();
	Stage.SetRenderTarget(RTarget, false);
	InitPostProcessing();

	return OskResult::SUCCESS;
}


Renderpass* RenderAPI::CreateNewRenderpass() {
	return new Renderpass(LogicalDevice);
}

RenderTarget* RenderAPI::CreateNewRenderTarget() {
	return new RenderTarget(this);
}
void RenderAPI::InitRenderTarget(RenderTarget* rtarget, ContentManager* content) {
	rtarget->Size = { (uint32_t)Window->GetRectangle().GetRectangleWidth(), (uint32_t)Window->GetRectangle().GetRectangleHeight() };
	rtarget->CreateSprite(content);

	//Image
	VULKAN::VulkanImageGen::CreateImage(&rtarget->RenderedSprite.Texture2D->Image, rtarget->Size, SwapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&rtarget->RenderedSprite.Texture2D->Image, SwapchainFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(rtarget->RenderedSprite.Texture2D->Image, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	rtarget->RenderedSprite.SpriteMaterial = GetMaterialSystem()->GetMaterial(DefaultMaterial2D_Name)->CreateInstance();
	rtarget->RenderedSprite.SpriteMaterial->SetTexture(rtarget->RenderedSprite.Texture2D);
	rtarget->RenderedSprite.SpriteMaterial->FlushUpdate();

	//Renderpass
	VULKAN::RenderpassAttachment rpassAttachment{};
	rpassAttachment.AddAttachment(SwapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	rpassAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	std::vector<VULKAN::RenderpassAttachment> attchments = { };
	rtarget->CreateRenderpass(attchments, &rpassAttachment);

	//Framebuffers
	rtarget->SetSize(rtarget->Size.X, rtarget->Size.Y, true, false);
	rtarget->CreateFramebuffers(4, &rtarget->RenderedSprite.Texture2D->Image.View, 1);
}

void RenderAPI::SetPresentMode(PresentMode mode) {
	targetPresentMode = mode;

	if (hasBeenInit)
		RecreateSwapchain();
}


PresentMode RenderAPI::GetCurrentPresentMode() const {
	return targetPresentMode;
}


void RenderAPI::ReloadShaders() {
	RecreateSwapchain();
}

Framebuffer* RenderAPI::CreateNewFramebuffer() {
	return new Framebuffer(this);
}

void RenderAPI::RenderFrame() {
	renderP_Unit.Start();

	double startTime = glfwGetTime();

	{
		DefaultCamera2D.Update();
		DefaultCamera3D.updateVectors();
		for (size_t i = 0; i < UniformBuffers.size(); i++) {
			void* data;
			vkMapMemory(LogicalDevice, UniformBuffers[i].Memory, 0, sizeof(UBO), 0, &data);
			UBO ubo{};
			ubo.view = DefaultCamera3D.GetView();
			ubo.projection = DefaultCamera3D.GetProjection();
			ubo.projection2D = DefaultCamera2D.projection;
			ubo.cameraPos = DefaultCamera3D.CameraTransform.GlobalPosition.ToGLM();

			memcpy(data, &ubo, sizeof(UBO));
			vkUnmapMemory(LogicalDevice, UniformBuffers[i].Memory);
		}

		updateCommandBuffers();
	}
	//Repreenta cual es la imagen que se va a renderizar.
	uint32_t nextImageIndex = 0;

	vkWaitForFences(LogicalDevice, 1, &fences[renderVars.currentImage], VK_TRUE, UINT64_MAX);
	//Adquiere la siguiente imágen sobre la que se va a renderizar.
	VkResult result = vkAcquireNextImageKHR(LogicalDevice, Swapchain, UINT64_MAX, ImageAvailableSemaphores[0], VK_NULL_HANDLE, &nextImageIndex);
	vkWaitForFences(LogicalDevice, 1, &fences[nextImageIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(LogicalDevice, 1, &fences[nextImageIndex]);

	//La ventana ha cambiado de tamaño.
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapchain();

		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("ERROR: obtener imagen.");

	//if (ImagesInFlight[nextImageIndex] != VK_NULL_HANDLE) {
	//	vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[nextImageIndex], VK_TRUE, UINT64_MAX);
	//}
	//ImagesInFlight[nextImageIndex] = InFlightFences[renderVars.currentImage];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Esperar a que la imagen esté disponible antes de renderizar.
	VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[0] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &ImageAvailableSemaphores[0];
	submitInfo.signalSemaphoreCount = 1;
	
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[nextImageIndex];

	//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
	VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[0] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//vkResetFences(LogicalDevice, 1, &fences[nextImageIndex]);

	result = vkQueueSubmit(GraphicsQ, 1, &submitInfo, fences[nextImageIndex]);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: submit queue.");

	//Presentar la imagen renderizada en la pantalla.
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { Swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &nextImageIndex;
	presentInfo.pResults = nullptr;

	vkWaitForFences(LogicalDevice, 1, &fences[nextImageIndex], VK_TRUE, UINT64_MAX);
	result = vkQueuePresentKHR(PresentQ, &presentInfo);

	//La ventana ha cambiado de tamaño.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderVars.hasFramebufferBeenResized) {
		RecreateSwapchain();
		renderVars.hasFramebufferBeenResized = false;
	}

	//vkQueueWaitIdle(PresentQ);

	renderVars.currentImage = (renderVars.currentImage + 1) % MAX_FRAMES_IN_FLIGHT;

	double endTime = glfwGetTime();

	double targetMS = 1 / FPSlimit;
	renderP_Unit.End();
	//if ((endTime - startTime) < targetMS)
		//std::this_thread::sleep_for(std::chrono::milliseconds((long)(targetMS * 1000 - (endTime - startTime) * 1000)));
}

SpriteBatch RenderAPI::CreateSpriteBatch() {
	SpriteBatch spriteBatch{};
	spriteBatch.renderer = this;

	return spriteBatch;
}


GraphicsPipeline* RenderAPI::CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	return new GraphicsPipeline(LogicalDevice, vertexPath, fragmentPath);
}

DescriptorPool* RenderAPI::CreateNewDescriptorPool() const {
	return new DescriptorPool(LogicalDevice, SwapchainImages.size());
}

DescriptorLayout* RenderAPI::CreateNewDescriptorLayout() const {
	return new DescriptorLayout(LogicalDevice);
}

DescriptorSet* RenderAPI::CreateNewDescriptorSet() const {
	return new DescriptorSet(LogicalDevice, SwapchainImages.size());
}

void RenderAPI::AddSpriteBatch(SpriteBatch* spriteBatch) {
	Stage.AddSpriteBatch(spriteBatch);
}

void RenderAPI::RemoveSpriteBatch(SpriteBatch* spriteBatch) {
	Stage.RemoveSpriteBatch(spriteBatch);
}

bool RenderAPI::checkValidationLayers() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}


void RenderAPI::createInstance(const std::string& appName, const Version& gameVersion) {
	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)gameVersion.Mayor, (int)gameVersion.Menor, (int)gameVersion.Parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION((int)ENGINE_VERSION_STAGE_NUMERIC, (int)ENGINE_VERSION_MINOR, (int)ENGINE_VERSION_BUILD_NUMERIC);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	//Create info.
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Establecemos las extensiones.
	//Extensiones de la ventana.
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Extensiones totales.
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef OSK_DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

#ifdef OSK_DEBUG
	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	if (checkValidationLayers()) {
		Logger::DebugLog("VALIDATION LAYERS: ON.");

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugConsoleCreateInfo{};
		debugConsoleCreateInfo = {};
		debugConsoleCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugConsoleCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugConsoleCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugConsoleCreateInfo.pfnUserCallback = DebugCallback;
		debugConsoleCreateInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugConsoleCreateInfo;
	}
	else {
		Logger::Log(LogMessageLevels::WARNING, "No se ha encontrado soporte para las capas de validación.");
	}
#endif

	Logger::DebugLog("Extensiones del renderizador: ");
	for (const auto& i : extensions)
		Logger::DebugLog(i);
	Logger::DebugLog("Capas de validación del renderizador: ");
	for (const auto& i : extensions)
		Logger::DebugLog(i);

	//Crear la instancia y error-handling.
	VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Crear instancia de Vulkan." + std::to_string(result));
}


void RenderAPI::setupDebugConsole() {
#ifdef OSK_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	//Crearlo.
	VkResult result = CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &debugConsole);
	if (result != VK_SUCCESS)
		Logger::Log(LogMessageLevels::BAD_ERROR, "ERROR: no se puede iniciar la consola de capas de validación.");

	Logger::DebugLog("Capas de validación activas.");
#endif
}


void RenderAPI::createSurface() {
	//Obtener la superficie.
	VkResult result = glfwCreateWindowSurface(Instance, Window->GetGLFWWindow(), nullptr, &Surface);

	//Error-handling.
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear superficie.");
}


void RenderAPI::getGPU() {
	//Obtiene el número de GPUs disponibles.
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(Instance, &count, nullptr);

	if (count == 0)
		Logger::Log(LogMessageLevels::CRITICAL_ERROR, "no se encuentra ninguna GPU compatible.");

	//Obtiene las GPUs.
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(Instance, &count, devices.data());

	//Comprobar la compatibilidad de las GPUs.
	//Obtener una GPU compatible.
	std::vector<GPUinfo> gpus{};
	for (const auto& gpu : devices)
		gpus.push_back(getGPUinfo(gpu));

	GPU = gpus[0].GPU;
	GPU_Info = gpus[0];
	vkGetPhysicalDeviceMemoryProperties(GPU, &MemoryProperties);

	if (GPU == nullptr)
		Logger::Log(LogMessageLevels::CRITICAL_ERROR, " no se encuentra ninguna GPU compatible.");
}


void RenderAPI::createLogicalDevice() {
	//Establecer las colas que vamos a usar.
	QueueFamilyIndices indices = findQueueFamilies(GPU);

	std::vector<VkDeviceQueueCreateInfo> createInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.GraphicsFamily.value(),
		indices.PresentFamily.value()
	};

	//Creación de las colas.
	float_t qPriority = 1.0f;
	for (auto& qFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo qCreateInfo{};
		qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qCreateInfo.queueFamilyIndex = qFamily;
		qCreateInfo.queueCount = 1;

		//Prioridades.
		qCreateInfo.pQueuePriorities = &qPriority;

		createInfos.push_back(qCreateInfo);
	}

	//Características que vamos a usar.
	VkPhysicalDeviceFeatures features{};
	features.samplerAnisotropy = VK_TRUE;

	//Crear el logical device.
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfos.size());
	createInfo.pQueueCreateInfos = createInfos.data();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(gpuExtensions.size());
	createInfo.ppEnabledExtensionNames = gpuExtensions.data();

	//Crear el logical device.
	VkResult result = vkCreateDevice(GPU, &createInfo, nullptr, &LogicalDevice);

	//Error-handling.
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear logical device.");

	//Obtener las colas.
	vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamily.value(), 0, &GraphicsQ);
	vkGetDeviceQueue(LogicalDevice, indices.PresentFamily.value(), 0, &PresentQ);
}


void RenderAPI::createSwapchain() {
	//Soporte del swapchain.
	SwapchainSupportDetails swapchainDetails = getSwapchainSupportDetails(GPU);

	//Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat = getSwapchainFormat(swapchainDetails.Formats);

	//Modo de pressentación.
	VkPresentModeKHR presentMode = getPresentMode(swapchainDetails.PresentModes);

	//tamaño.
	VkExtent2D extent = getSwapchainExtent(swapchainDetails.SurfaceCapabilities);

	//Número de imágenes en el swapchain.
	uint32_t imageCount = swapchainDetails.SurfaceCapabilities.minImageCount + 1;

	//Asegurarnos de que no hay más de las soportadas.
	if (swapchainDetails.SurfaceCapabilities.maxImageCount > 0 && imageCount > swapchainDetails.SurfaceCapabilities.maxImageCount)
		imageCount = swapchainDetails.SurfaceCapabilities.maxImageCount;

	//Create info.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Colas.
	//Cómo se maneja el swapchain.
	const QueueFamilyIndices indices = findQueueFamilies(GPU);
	const uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };
	if (indices.GraphicsFamily != indices.PresentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapchainDetails.SurfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; //Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	//Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(LogicalDevice, &createInfo, nullptr, &Swapchain);

	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear swapchain.");

	//Obtener el número final de imágenes del swapchain.
	vkGetSwapchainImagesKHR(LogicalDevice, Swapchain, &imageCount, nullptr);

	//Obtener las imágenes del swapchain.
	SwapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(LogicalDevice, Swapchain, &imageCount, SwapchainImages.data());

	//Actualizar las variables de la clase.
	SwapchainFormat = surfaceFormat.format;
	SwapchainExtent = extent;
}


void RenderAPI::createSwapchainImageViews() {
	SwapchainImageViews.resize(SwapchainImages.size());

	//Por cada imagen...
	for (uint32_t i = 0; i < SwapchainImageViews.size(); i++) {
		//Create info.
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapchainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(LogicalDevice, &createInfo, nullptr, &SwapchainImageViews[i]);
	}
}


void RenderAPI::createRenderpass() {
	renderpass = new Renderpass(LogicalDevice);

	RenderpassAttachment clrAttachment{};
	clrAttachment.AddAttachment(SwapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	clrAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	RenderpassAttachment dpthAttachment{};
	dpthAttachment.AddAttachment(getDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	dpthAttachment.CreateReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	std::vector<RenderpassAttachment> attchments = { clrAttachment };

	RenderpassSubpass sbPass{};
	sbPass.SetColorAttachments(attchments);
	sbPass.SetDepthStencilAttachment(dpthAttachment);
	sbPass.SetPipelineBindPoint();
	SubpassDependency dep;
	dep.VulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.VulkanDependency.dstSubpass = 0;
	dep.VulkanDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep.VulkanDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep.VulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.VulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep.VulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	SubpassDependency dep2;
	dep2.VulkanDependency.srcSubpass = 0;
	dep2.VulkanDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	dep2.VulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep2.VulkanDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep2.VulkanDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep2.VulkanDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep2.VulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	sbPass.AddDependency(dep);
	sbPass.AddDependency(dep2);

	renderpass->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
	renderpass->AddSubpass(sbPass);
	renderpass->AddAttachment(clrAttachment);
	renderpass->AddAttachment(dpthAttachment);
	renderpass->Create();
}

void RenderAPI::createCommandPool() {
	QueueFamilyIndices indices = findQueueFamilies(GPU);

	//Para la graphics q.
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &CommandPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear command pool.");
}


void RenderAPI::createDepthResources() {
	VkFormat depthFormat = getDepthFormat();
	VulkanImageGen::CreateImage(&DepthImage, { RenderTargetSizeX, RenderTargetSizeY }, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VulkanImageGen::CreateImageView(&DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
}


void RenderAPI::createFramebuffers() {
	Framebuffers.resize(SwapchainImageViews.size());

	//Crear los framebuffers.
	for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
		Framebuffer* framebuffer = new Framebuffer(this);

		framebuffer->AddImageView(SwapchainImageViews[i]);
		framebuffer->AddImageView(&DepthImage);
		
		framebuffer->Create(renderpass, SwapchainExtent.width, SwapchainExtent.height);
		
		Framebuffers[i] = framebuffer;
	}
}


void RenderAPI::createGlobalImageSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//	VK_FILTER_NEAREST: pixelado.
	//	VK_FILTER_LINEAR: suavizado.
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	//Qué hacer cuando se accede a unas coordenadas fuera de la textura.
	//	VK_SAMPLER_ADDRESS_MODE_REPEAT: la textura se repite.
	//	VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: la textura se repite en fomra de espejo.	
	//	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: se alarga el último pixel.
	//	VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: se alarga el primer pixel.
	//	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: devuelve un color fijo.
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	//Sampler NO NECESITA SER POR IMAGEN, PUEDE HABER UNO PARA TODAS.

	VkResult result = vkCreateSampler(LogicalDevice, &samplerInfo, nullptr, &GlobalImageSampler);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear sampler.");
}


void RenderAPI::createDefaultUniformBuffers() {
	VkDeviceSize size = sizeof(UBO);
	UniformBuffers.resize(SwapchainImages.size());

	for (uint32_t i = 0; i < UniformBuffers.size(); i++) {
		UniformBuffers[i] = CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		UniformBuffers[i].Allocate(size);
	}
}


void RenderAPI::createDescriptorPool() {

}


void RenderAPI::createSyncObjects() {
	ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	ImagesInFlight.resize(SwapchainImages.size(), nullptr);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult result;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		result = vkCreateSemaphore(LogicalDevice, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [0].");

		result = vkCreateSemaphore(LogicalDevice, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [1].");

		result = vkCreateFence(LogicalDevice, &fenceInfo, nullptr, &InFlightFences[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [2].");
	}

	fences = new VkFence[SwapchainImages.size()];
	for (uint32_t i = 0; i < SwapchainImages.size(); i++) {
		vkCreateFence(LogicalDevice, &fenceInfo, nullptr, &fences[i]);
	}
}


void RenderAPI::createCommandBuffers() {
	CommandBuffers.resize(Framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	VkResult result = vkAllocateCommandBuffers(LogicalDevice, &allocInfo, CommandBuffers.data());
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear command buffers.");
}


void RenderAPI::closeSwapchain() {
	DepthImage.Destroy();

	for (auto& i : Framebuffers)
		delete i;
	Framebuffers.clear();

	for (const auto& i : SwapchainImageViews)
		vkDestroyImageView(LogicalDevice, i, nullptr);

	vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
}


void RenderAPI::Close() {
	closeSwapchain();
	ClosePostProcessing();

	delete Content;

	delete RTarget;

	delete MSystem;

	vkDestroySampler(LogicalDevice, GlobalImageSampler, nullptr);

	for (auto& i : UniformBuffers)
		i.Free();

	Sprite::IndexBuffer.Free();

	vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
		vkDestroyFence(LogicalDevice, fences[i], nullptr);
	}

	vkDestroyDevice(LogicalDevice, nullptr);
#ifdef OSK_DEBUG
	if (checkValidationLayers())
		DestroyDebugUtilsMessengerEXT(Instance, debugConsole, nullptr);
#endif
	vkDestroySurfaceKHR(Instance, Surface, nullptr);

	//Destruir la instancia.
	vkDestroyInstance(Instance, nullptr);
}


void RenderAPI::RecreateSwapchain() {
	int32_t width = 0;
	int32_t height = 0;
	glfwGetFramebufferSize(Window->GetGLFWWindow(), &width, &height);

	//Si está minimizado, esperar.
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(Window->GetGLFWWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(LogicalDevice);

	closeSwapchain();

	RenderTargetSizeX = Window->ScreenSizeX * RenderResolutionMultiplier;
	RenderTargetSizeY = Window->ScreenSizeY * RenderResolutionMultiplier;

	createSwapchain();
	createSwapchainImageViews();
	createRenderpass();
	
	createDepthResources();

	createFramebuffers();

	createRenderTarget();
	RecreatePostProcessing();
}


void RenderAPI::createRenderTarget() {
	RenderTargetSizeX = Window->ScreenSizeX * RenderResolutionMultiplier;
	RenderTargetSizeY = Window->ScreenSizeY * RenderResolutionMultiplier;

	RTarget->Clear(false);

	RTarget->SetFormat(SwapchainFormat);
	RTarget->SetSize(RenderTargetSizeX, RenderTargetSizeY, true, false);

	VkImageView views[] = { RTarget->RenderedSprite.Texture2D->Image.View, DepthImage.View };
	RTarget->CreateFramebuffers(SwapchainImages.size() + 1, views, 2);

	Logger::DebugLog("Recreated swapchain.");
	Logger::DebugLog("Resolution multiplier = " + std::to_string(RenderResolutionMultiplier) + ".");
	Logger::DebugLog("Renderer resolution = " + ToString(Vector2ui(RenderTargetSizeX, RenderTargetSizeY)) + ".");
	Logger::DebugLog("Output resolution = " + ToString(Vector2ui(Window->ScreenSizeX, Window->ScreenSizeY)) + ".");

	RTarget->RenderedSprite.SpriteTransform.SetPosition({ 0.0f });
	RTarget->RenderedSprite.SpriteTransform.SetScale(Vector2ui{ RenderTargetSizeX, RenderTargetSizeY }.ToVector2f() / RenderResolutionMultiplier);
}

VulkanBuffer RenderAPI::CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const {
	VulkanBuffer buffer;
	buffer.Create(LogicalDevice, usage, prop, MemoryProperties);

	return buffer;
}

void RenderAPI::CreateDynamicUBO(VulkanBuffer& buffer, VkDeviceSize sizeOfStruct, uint32_t numberOfInstances) const {
	size_t minAlignment = GPU_Info.minAlignment;
	size_t alignment = sizeOfStruct;
	if (minAlignment > 0) {
		alignment = (alignment + minAlignment - 1) & ~(minAlignment - 1);
	}

	size_t bufferSize = alignment * numberOfInstances;

	buffer = CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	buffer.Alignment = alignment;
	buffer.Allocate(bufferSize);
	buffer.DynamicSize = sizeOfStruct;
}


void RenderAPI::SetRenderizableScene(RenderizableScene* scene) {
	Scene = scene;
	Scene->TargetRenderpass = RTarget->VRenderpass;
}

void RenderAPI::AddStage(RenderStage* stage) {
	Stages.push_back(stage);
}

void RenderAPI::RemoveStage(RenderStage* stage) {
	Stages.remove(stage);
}

void RenderAPI::AddSingleTimeStage(RenderStage* stage) {
	SingleTimeStages.push_back(stage);
}

void RenderAPI::DrawStage(RenderStage* stage, VkCommandBuffer cmdBuffer, uint32_t iteration) {
	if (!stage->RTarget)
		return;
	
	if (stage->Scene != nullptr) {
		stage->Scene->UpdateLightsBuffers();

		stage->Scene->DrawShadows(cmdBuffer, iteration);

		//for (auto& cubeMap : stage->Scene->cubeShadowMaps)
			//stage->Scene->DrawPointShadows(cmdBuffer, iteration, cubeMap);
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = stage->RTarget->VRenderpass->VulkanRenderpass;
	renderPassInfo.framebuffer = stage->RTarget->TargetFramebuffers[iteration]->framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };

	renderPassInfo.renderArea.extent = { stage->RTarget->Size.X, stage->RTarget->Size.Y };
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
	clearValues[1] = { 1.0f, 0.0f }; //Depth.
	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	stage->RTarget->TransitionToRenderTarget(&cmdBuffer);
	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	SetViewport(cmdBuffer);

	if (stage->Scene != nullptr) {
		stage->Scene->Draw(cmdBuffer, iteration);
	}

	for (auto& spriteBatch : stage->SpriteBatches) {
		
		if (!spriteBatch->spritesToDraw.IsEmpty()) {
			
			stage->RTarget->Pipelines[0]->Bind(cmdBuffer);
			vkCmdBindIndexBuffer(cmdBuffer, Sprite::IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = Sprite::Indices.size();

			for (auto& sprite : spriteBatch->spritesToDraw) {
				SpriteContainer sprite = spriteBatch->spritesToDraw.Dequeue();

				VkBuffer vertexBuffers[] = { sprite.VertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

				sprite.SpriteMaterial->GetDescriptorSet()->Bind(cmdBuffer, stage->RTarget->Pipelines[0], iteration);

				PushConst2D pConst = sprite.PConst;
				vkCmdPushConstants(cmdBuffer, stage->RTarget->Pipelines[0]->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
			}
		}
	}

	vkCmdEndRenderPass(cmdBuffer);
	stage->RTarget->TransitionToTexture(&cmdBuffer);
}

void RenderAPI::updateCommandBuffers() {
	updateCmdP_Unit.Start();

	for (auto& spriteBatch : Stage.SpriteBatches) {
		for (auto& i : spriteBatch->spritesToDraw) {
			if (i.hasChanged)
				updateSpriteVertexBuffer(i);
		}
	}

	for (size_t i = 0; i < CommandBuffers.size(); i++) {
		vkResetCommandBuffer(CommandBuffers[i], 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(CommandBuffers[i], &beginInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar command buffer.");

		for (auto& stage : SingleTimeStages)
			DrawStage(stage, CommandBuffers[i], i);

		for (auto& stage : Stages)
			DrawStage(stage, CommandBuffers[i], i);
				
		if (RSystem) {
			RSystem->OnDraw(CommandBuffers[i], i);
		}

		PostProcessingSettings.ScreenSizeX = (int)Window->ScreenSizeX;
		PostProcessingSettings.ScreenSizeY = (int)Window->ScreenSizeY;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ScreenRenderpass->VulkanRenderpass;
		renderPassInfo.framebuffer = Framebuffers[i]->framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = SwapchainExtent;
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		//Comenzar el renderizado.
		vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		SetViewport(CommandBuffers[i], -(int32_t)Window->ScreenSizeX, -(int32_t)Window->ScreenSizeY, (uint32_t)Window->ScreenSizeX * 2, (uint32_t)Window->ScreenSizeY * 2);

		ScreenGraphicsPipeline->Bind(CommandBuffers[i]);
		vkCmdBindIndexBuffer(CommandBuffers[i], Sprite::IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
		const size_t indicesSize = Sprite::Indices.size();

		VkBuffer vertexBuffers[] = { RTarget->RenderedSprite.VertexBuffer.Buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);

		ScreenDescriptorSet->Bind(CommandBuffers[i], ScreenGraphicsPipeline, i);

		vkCmdPushConstants(CommandBuffers[i], ScreenGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PostProcessingSettings_t), &PostProcessingSettings);
		vkCmdDrawIndexed(CommandBuffers[i], indicesSize, 1, 0, 0, 0);

		vkCmdEndRenderPass(CommandBuffers[i]);

		result = vkEndCommandBuffer(CommandBuffers[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar renderpass.");

		updateCmdP_Unit.End();
	}

	SingleTimeStages.clear();
}

void RenderAPI::createSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	sprite->VertexBuffer = CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	sprite->VertexBuffer.Allocate(size);
	
	updateSpriteVertexBuffer(sprite);
}

void RenderAPI::createSpriteIndexBuffer() const {
	VkDeviceSize size = sizeof(Sprite::Indices[0]) * Sprite::Indices.size();

	VulkanBuffer stagingBuffer = CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.Allocate(size);

	stagingBuffer.Write(Sprite::Indices.data(), size);

	Sprite::IndexBuffer = CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	Sprite::IndexBuffer.Allocate(size);
	CopyBuffer(stagingBuffer, Sprite::IndexBuffer, size);

	stagingBuffer.Free();
}


void RenderAPI::updateSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	void* data;
	vkMapMemory(LogicalDevice, sprite->VertexBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Vertices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, sprite->VertexBuffer.Memory);

	sprite->hasChanged = false;
}

void RenderAPI::updateSpriteVertexBuffer(SpriteContainer& sprite) const {
	VkDeviceSize size = sizeof(Vertex) * 4;

	void* data;
	vkMapMemory(LogicalDevice, sprite.VertexMemory, 0, size, 0, &data);
	memcpy(data, sprite.Vertices, (size_t)size);
	vkUnmapMemory(LogicalDevice, sprite.VertexMemory);

	sprite.hasChanged = false;
}


//Copia el contenido de un buffer a otro buffer.
void RenderAPI::CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = sourceOffset;
	copyRegion.dstOffset = destinationOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, source.Buffer, destination.Buffer, 1, &copyRegion);

	endSingleTimeCommandBuffer(cmdBuffer);
}


VkCommandBuffer RenderAPI::beginSingleTimeCommandBuffer() const {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(LogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}


void RenderAPI::endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const {
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(GraphicsQ, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQ);

	vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &cmdBuffer);
}


//Obtiene el tipo de memoria indicado.
uint32_t RenderAPI::getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const {
	//Tipos de memoria disponibles.
	for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; i++)
		if (memoryTypeFilter & (1 << i) && (MemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;

	return -1;

	throw std::runtime_error("failed to find suitable memory type!");
}


VkFormat RenderAPI::getDepthFormat() const {
	return getSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

GPUinfo RenderAPI::getGPUinfo(VkPhysicalDevice gpu) const {
	GPUinfo info{};
	info.GPU = gpu;
	//Obtiene las propiedades de la gpu.
	vkGetPhysicalDeviceProperties(gpu, &info.Properties);
	//Obtiene las características de la GPU.
	vkGetPhysicalDeviceFeatures(gpu, &info.Features);

	//Comprobar soporte de colas.
	info.Families = findQueueFamilies(gpu);

	//Comprobar soporte de swapchain.
	bool swapchainSupported = false;

	info.ShapchainSupport = getSwapchainSupportDetails(gpu);
	swapchainSupported = !info.ShapchainSupport.PresentModes.empty() && !info.ShapchainSupport.Formats.empty();

	info.IsSuitable = info.Families.IsComplete() && checkGPUextensionSupport(gpu) && swapchainSupported && info.Features.samplerAnisotropy;
	info.minAlignment = info.Properties.limits.minUniformBufferOffsetAlignment;

	return info;
}


VkPresentModeKHR RenderAPI::getPresentMode(const std::vector<VkPresentModeKHR>& modes) const {
	VkPresentModeKHR finalPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkPresentModeKHR targetPresentMode = translatePresentMode(this->targetPresentMode);

	for (const auto& mode : modes)
		if (mode == targetPresentMode)
			finalPresentMode = mode;

	return finalPresentMode;
}


VkExtent2D RenderAPI::getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
	//Si está inicializado, devolver.
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;

	int width;
	int height;
	glfwGetFramebufferSize(Window->GetGLFWWindow(), &width, &height);

	VkExtent2D extent{
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};

	//Que no sea más grande de lo soportado.
	extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
	extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

	return extent;
}


VkFormat RenderAPI::getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
	for (VkFormat format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(GPU, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			return format;
	}

	throw std::runtime_error("ERROR: GetDepthSupportedFormat");
}


SwapchainSupportDetails RenderAPI::getSwapchainSupportDetails(VkPhysicalDevice gpu) const {
	SwapchainSupportDetails details;

	//Obtener las capacidades.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, Surface, &details.SurfaceCapabilities);

	//Número de formatos soportados.
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, Surface, &formatCount, nullptr);

	//Obtener formatos soportados.
	details.Formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, Surface, &formatCount, details.Formats.data());

	//Números de modos de presentación.
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, Surface, &presentModeCount, nullptr);

	//Obtener modos de presentación.
	details.PresentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, Surface, &formatCount, details.PresentModes.data());

	return details;
}


QueueFamilyIndices RenderAPI::findQueueFamilies(VkPhysicalDevice gpu) const {
	QueueFamilyIndices indices{};

	//Número de colas.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

	//Obtener las colas.
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilies.data());

	//Comprobar el soporte de distintos tipos de cola.
	int i = 0;
	for (const auto& q : queueFamilies) {
		if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.GraphicsFamily = i;

		//Soporte para presentación.
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, Surface, &presentSupport);

		if (presentSupport)
			indices.PresentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}


VkResult RenderAPI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}


VkBool32 RenderAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	//Message severity:
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

	//Tipos de mensaje:
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

	LogMessageLevels level;

	switch (messageType) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			level = LogMessageLevels::INFO;
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			level = LogMessageLevels::WARNING;
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			level = LogMessageLevels::BAD_ERROR;
			break;

		default:
			level = LogMessageLevels::WARNING;
			break;
	}

	Logger::Log(level, std::string(pCallbackData->pMessage) + "\n");

	return VK_FALSE;
}

void RenderAPI::InitPostProcessing() {
	//return;
	FinalRenderTarget = CreateNewRenderTarget();
	FinalRenderTarget->SetFormat(SwapchainFormat);
	FinalRenderTarget->SetSize((uint32_t)Window->ScreenSizeX, (uint32_t)Window->ScreenSizeY);
	FinalRenderTarget->CreateSprite(Content);

	VULKAN::VulkanImageGen::CreateImage(&FinalRenderTarget->RenderedSprite.Texture2D->Image, FinalRenderTarget->Size, SwapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&FinalRenderTarget->RenderedSprite.Texture2D->Image, SwapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(FinalRenderTarget->RenderedSprite.Texture2D->Image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	VULKAN::VulkanImageGen::TransitionImageLayout(&FinalRenderTarget->RenderedSprite.Texture2D->Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 0);

	VULKAN::VulkanImageGen::CreateImageSampler(FinalRenderTarget->RenderedSprite.Texture2D->Image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

	ScreenRenderpass = renderpass;

	ScreenDescriptorPool = CreateNewDescriptorPool();
	ScreenDescriptorPool->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	ScreenDescriptorPool->Create(1);

	ScreenDescriptorLayout = CreateNewDescriptorLayout();
	ScreenDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	ScreenDescriptorLayout->DPool = ScreenDescriptorPool;
	ScreenDescriptorLayout->Create();

	ScreenDescriptorSet = CreateNewDescriptorSet();
	ScreenDescriptorSet->Create(ScreenDescriptorLayout, ScreenDescriptorPool, true);
	ScreenDescriptorSet->AddImage(&RTarget->RenderedSprite.Texture2D->Image, RTarget->RenderedSprite.Texture2D->Image.Sampler, 0);
	ScreenDescriptorSet->Update();

	ScreenGraphicsPipeline = CreateNewGraphicsPipeline("shaders/VK_Post/vert.spv", "shaders/VK_Post/frag.spv");
	ScreenGraphicsPipeline->SetViewport({ -(float)SwapchainExtent.width, -(float)SwapchainExtent.height, (float)SwapchainExtent.width * 2, (float)SwapchainExtent.height * 2 });
	ScreenGraphicsPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	ScreenGraphicsPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	ScreenGraphicsPipeline->SetDepthStencil(false);
	ScreenGraphicsPipeline->SetPushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PostProcessingSettings_t));
	ScreenGraphicsPipeline->SetLayout(&ScreenDescriptorLayout->VulkanDescriptorSetLayout);
	ScreenGraphicsPipeline->Create(ScreenRenderpass);

	FinalRenderTarget->VRenderpass = ScreenRenderpass;
	VkImageView views[] = { FinalRenderTarget->RenderedSprite.Texture2D->Image.View, DepthImage.View };
	FinalRenderTarget->CreateFramebuffers(4, views, 2);
	updateSpriteVertexBuffer(&RTarget->RenderedSprite);
}

void RenderAPI::RecreatePostProcessing() {
	ClosePostProcessing();
	InitPostProcessing();
}

void RenderAPI::ClosePostProcessing() {
	SafeDelete(&ScreenDescriptorLayout);
	SafeDelete(&ScreenDescriptorSet);
	SafeDelete(&ScreenGraphicsPipeline);
	SafeDelete(&FinalRenderTarget);
}