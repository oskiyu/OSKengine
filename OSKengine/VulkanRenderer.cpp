#include "VulkanRenderer.h"

#include "Log.h"
#include "OSKmacros.h"
#include "OSKsettings.h"
#include <vector>

#include "VulkanImageGen.h"
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
using namespace OSK::Memory;
using namespace OSK::VULKAN;

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;


void RenderAPI::SetViewport(VkCommandBuffer& cmdBuffer, int32_t x, int32_t y, uint32_t sizeX, uint32_t sizeY) const {
	VkViewport viewport{};
	viewport.x = x;
	viewport.y = y;
	if (sizeX != 0)
		viewport.width = sizeX;
	else
		viewport.width = window->GetSize().X;
	if (sizeY != 0)
		viewport.height = sizeY;
	else
		viewport.height = window->GetSize().Y;
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

void RenderAPI::Init(const std::string& appName, const Version& gameVersion) {
	VULKAN::VulkanImageGen::SetRenderer(this);

	createInstance(appName, gameVersion);

	if (checkValidationLayers())
		setupDebugConsole();

	createSurface();
	getGPU();
	createLogicalDevice();
	createSwapchain();
	createSwapchainImageViews();

	materialSystem = new MaterialSystem(this);

	createRenderpass();
	
	renderTargetSizeX = swapchainExtent.width * renderResolutionMultiplier;
	renderTargetSizeY = swapchainExtent.height * renderResolutionMultiplier;

	{
		RenderpassAttachment clrAttachment{};
		clrAttachment.AddAttachment(swapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);//
		clrAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		RenderpassAttachment dpthAttachment{};
		dpthAttachment.AddAttachment(getDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		dpthAttachment.CreateReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		std::vector<RenderpassAttachment> attchments = { clrAttachment };

		renderTargetBeforePostProcessing->CreateRenderpass(attchments, &dpthAttachment);
	}

	createCommandPool();
	createGlobalImageSampler();
	createDefaultUniformBuffers();
	ContentManager::DefaultTexture = content->LoadTexture(ContentManager::DEFAULT_TEXTURE_PATH);
	Material::DefaultTexture = ContentManager::DefaultTexture;

	//MATERIALS
	{
		materialSystem->RegisterMaterial(defaultMaterial2D_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture" });
		materialSystem->GetMaterial(defaultMaterial2D_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.cullMode = PolygonCullMode::NONE;
		pipelineInfo.vertexPath = Settings.vertexShaderPath2D;
		pipelineInfo.fragmentPath = Settings.fragmentShaderPath2D;
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst2D) });
		materialSystem->GetMaterial(defaultMaterial2D_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		materialSystem->RegisterMaterial(defaultMaterial3D_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Animation" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Model" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Albedo" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::FRAGMENT, "Lights" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Specular" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Shadows" });
		materialSystem->GetMaterial(defaultMaterial3D_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.vertexPath = Settings.vertexShaderPath3D;
		pipelineInfo.fragmentPath = Settings.fragmentShaderPath3D;
		pipelineInfo.useDepthStencil = true;
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst3D) });
		materialSystem->GetMaterial(defaultMaterial3D_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		materialSystem->RegisterMaterial(defaultSkyboxMaterial_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture" });
		materialSystem->GetMaterial(defaultSkyboxMaterial_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.cullMode = PolygonCullMode::FRONT;
		pipelineInfo.frontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;
		pipelineInfo.vertexPath = Settings.skyboxVertexPath;
		pipelineInfo.fragmentPath = Settings.skyboxFragmentPath;
		pipelineInfo.useDepthStencil = false;
		materialSystem->GetMaterial(defaultSkyboxMaterial_Name)->SetPipelineSettings(pipelineInfo);
	}
	{
		materialSystem->RegisterMaterial(defaultShadowsMaterial_Name);
		MaterialBindingLayout layout;
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" });
		layout.push_back({ MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Animation" });
		layout.push_back({ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Lights" });
		materialSystem->GetMaterial(defaultShadowsMaterial_Name)->SetLayout(layout);
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.polygonMode = PolygonMode::FILL;
		pipelineInfo.cullMode = PolygonCullMode::FRONT;
		pipelineInfo.frontFaceType = PolygonFrontFaceType::CLOCKWISE;
		pipelineInfo.vertexPath = "shaders/VK_Shadows/vert.spv";
		pipelineInfo.fragmentPath = "shaders/VK_Shadows/frag.spv";
		pipelineInfo.useDepthStencil = true;
		materialSystem->GetMaterial(defaultShadowsMaterial_Name)->SetPipelineSettings(pipelineInfo);
	}
	//!MATERIALS

	createDepthResources();
	createFramebuffers();
	createSyncObjects();
	createCommandBuffers();

	createSpriteIndexBuffer();

	Skybox::Model = content->LoadModelData("models/Skybox/cube.obj");

	defaultCamera2D = Camera2D(window);
	defaultCamera3D.window = window;

	hasBeenInit = true;

	content->LoadSprite(OSKengineIconSprite, "Resources/OSKengine_icon_lowres_48.png");
	content->LoadSprite(OSK_IconSprite, "Resources/OSK_icon_lowres.png");

	renderTargetBeforePostProcessing->CreateSprite(content);

	//Image
	renderTargetBeforePostProcessing->size.X = renderTargetSizeX;
	renderTargetBeforePostProcessing->size.Y = renderTargetSizeY;
	VULKAN::VulkanImageGen::CreateImage(&renderTargetBeforePostProcessing->renderedSprite.texture->image, renderTargetBeforePostProcessing->size, swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&renderTargetBeforePostProcessing->renderedSprite.texture->image, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(renderTargetBeforePostProcessing->renderedSprite.texture->image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	renderTargetBeforePostProcessing->renderedSprite.material->SetTexture(renderTargetBeforePostProcessing->renderedSprite.texture);
	renderTargetBeforePostProcessing->renderedSprite.material->FlushUpdate();
	//renderTargetBeforePostProcessing->renderedSprite.tra
	
	createRenderTarget();
	InitPostProcessing();
}


Renderpass* RenderAPI::CreateNewRenderpass() {
	return new Renderpass(logicalDevice);
}

RenderTarget* RenderAPI::CreateNewRenderTarget() {
	return new RenderTarget(this);
}
void RenderAPI::InitRenderTarget(RenderTarget* rtarget, ContentManager* content) {
	rtarget->size = { (uint32_t)window->GetRectangle().GetRectangleWidth(), (uint32_t)window->GetRectangle().GetRectangleHeight() };
	rtarget->CreateSprite(content);

	//Image
	VULKAN::VulkanImageGen::CreateImage(&rtarget->renderedSprite.texture->image, rtarget->size, swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&rtarget->renderedSprite.texture->image, swapchainFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(rtarget->renderedSprite.texture->image, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	rtarget->renderedSprite.material = GetMaterialSystem()->GetMaterial(defaultMaterial2D_Name)->CreateInstance();
	rtarget->renderedSprite.material->SetTexture(rtarget->renderedSprite.texture);
	rtarget->renderedSprite.material->FlushUpdate();

	//Renderpass
	VULKAN::RenderpassAttachment rpassAttachment{};
	rpassAttachment.AddAttachment(swapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	rpassAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	std::vector<VULKAN::RenderpassAttachment> attchments = { };
	rtarget->CreateRenderpass(attchments, &rpassAttachment);

	//Framebuffers
	rtarget->SetSize(rtarget->size.X, rtarget->size.Y, true);
	rtarget->CreateFramebuffers(4, &rtarget->renderedSprite.texture->image.view, 1);
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
	renderProfilingUnit.Start();

	double startTime = glfwGetTime();

	{
		defaultCamera2D.Update();
		defaultCamera3D.updateVectors();
		for (size_t i = 0; i < uniformBuffers.size(); i++) {
			void* data;
			vkMapMemory(logicalDevice, uniformBuffers[i].memory, 0, sizeof(UBO), 0, &data);
			UBO ubo{};
			ubo.view = defaultCamera3D.GetView();
			ubo.projection = defaultCamera3D.GetProjection();
			ubo.cameraPos = defaultCamera3D.GetTransform()->GetPosition().ToGLM();

			memcpy(data, &ubo, sizeof(UBO));
			vkUnmapMemory(logicalDevice, uniformBuffers[i].memory);
		}

		updateCommandBuffers();
	}
	//Repreenta cual es la imagen que se va a renderizar.
	uint32_t nextImageIndex = 0;

	vkWaitForFences(logicalDevice, 1, &fences[renderVars.currentImage], VK_TRUE, UINT64_MAX);
	//Adquiere la siguiente imágen sobre la que se va a renderizar.
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphores[0], VK_NULL_HANDLE, &nextImageIndex);
	vkWaitForFences(logicalDevice, 1, &fences[nextImageIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice, 1, &fences[nextImageIndex]);

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
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[0] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &imageAvailableSemaphores[0];
	submitInfo.signalSemaphoreCount = 1;
	
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[nextImageIndex];

	//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[0] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//vkResetFences(LogicalDevice, 1, &fences[nextImageIndex]);

	result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences[nextImageIndex]);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: submit queue.");

	//Presentar la imagen renderizada en la pantalla.
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &nextImageIndex;
	presentInfo.pResults = nullptr;

	vkWaitForFences(logicalDevice, 1, &fences[nextImageIndex], VK_TRUE, UINT64_MAX);
	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	//La ventana ha cambiado de tamaño.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderVars.hasFramebufferBeenResized) {
		RecreateSwapchain();
		renderVars.hasFramebufferBeenResized = false;
	}

	//vkQueueWaitIdle(PresentQ);

	renderVars.currentImage = (renderVars.currentImage + 1) % MAX_FRAMES_IN_FLIGHT;

	double endTime = glfwGetTime();

	double targetMS = 1 / fpsLimit;
	renderProfilingUnit.End();
	//if ((endTime - startTime) < targetMS)
		//std::this_thread::sleep_for(std::chrono::milliseconds((long)(targetMS * 1000 - (endTime - startTime) * 1000)));
}

SpriteBatch RenderAPI::CreateSpriteBatch() {
	SpriteBatch spriteBatch{};
	spriteBatch.renderer = this;

	return spriteBatch;
}


GraphicsPipeline* RenderAPI::CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	return new GraphicsPipeline(logicalDevice, vertexPath, fragmentPath);
}

DescriptorPool* RenderAPI::CreateNewDescriptorPool() const {
	return new DescriptorPool(logicalDevice, swapchainImages.size());
}

DescriptorLayout* RenderAPI::CreateNewDescriptorLayout() const {
	return new DescriptorLayout(logicalDevice);
}

DescriptorSet* RenderAPI::CreateNewDescriptorSet() const {
	return new DescriptorSet(logicalDevice, swapchainImages.size());
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
	appInfo.applicationVersion = VK_MAKE_VERSION((int)gameVersion.mayor, (int)gameVersion.menor, (int)gameVersion.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION((int)ENGINE_VERSION_STAGE_NUMERIC, (int)ENGINE_VERSION_NUMERIC, (int)ENGINE_VERSION_BUILD_NUMERIC);
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

	/*Logger::DebugLog("Extensiones del renderizador: ");
	for (const auto& i : extensions)
		Logger::DebugLog(i);
	Logger::DebugLog("Capas de validación del renderizador: ");
	for (const auto& i : validationLayers)
		Logger::DebugLog(i);*/

	//Crear la instancia y error-handling.
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
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
	VkResult result = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugConsole);
	if (result != VK_SUCCESS)
		Logger::Log(LogMessageLevels::BAD_ERROR, "ERROR: no se puede iniciar la consola de capas de validación.");

	Logger::DebugLog("Capas de validación activas.");
#endif
}


void RenderAPI::createSurface() {
	//Obtener la superficie.
	VkResult result = glfwCreateWindowSurface(instance, window->window, nullptr, &surface);

	//Error-handling.
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear superficie.");
}


void RenderAPI::getGPU() {
	//Obtiene el número de GPUs disponibles.
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);

	if (count == 0)
		Logger::Log(LogMessageLevels::CRITICAL_ERROR, "no se encuentra ninguna GPU compatible.");

	//Obtiene las GPUs.
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(instance, &count, devices.data());

	//Comprobar la compatibilidad de las GPUs.
	//Obtener una GPU compatible.
	std::vector<GPUinfo> gpus{};
	for (const auto& gpu : devices)
		gpus.push_back(getGPUinfo(gpu));

	gpu = gpus[0].gpu;
	gpuInfo = gpus[0];
	vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

	if (gpu == nullptr)
		Logger::Log(LogMessageLevels::CRITICAL_ERROR, " no se encuentra ninguna GPU compatible.");
}


void RenderAPI::createLogicalDevice() {
	//Establecer las colas que vamos a usar.
	QueueFamilyIndices indices = findQueueFamilies(gpu);

	std::vector<VkDeviceQueueCreateInfo> createInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
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
	VkResult result = vkCreateDevice(gpu, &createInfo, nullptr, &logicalDevice);

	//Error-handling.
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear logical device.");

	//Obtener las colas.
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}


void RenderAPI::createSwapchain() {
	//Soporte del swapchain.
	SwapchainSupportDetails swapchainDetails = getSwapchainSupportDetails(gpu);

	//Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat = getSwapchainFormat(swapchainDetails.formats);

	//Modo de pressentación.
	VkPresentModeKHR presentMode = getPresentMode(swapchainDetails.presentModes);

	//tamaño.
	VkExtent2D extent = getSwapchainExtent(swapchainDetails.surfaceCapabilities);

	//Número de imágenes en el swapchain.
	uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;

	//Asegurarnos de que no hay más de las soportadas.
	if (swapchainDetails.surfaceCapabilities.maxImageCount > 0 && imageCount > swapchainDetails.surfaceCapabilities.maxImageCount)
		imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;

	//Create info.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Colas.
	//Cómo se maneja el swapchain.
	const QueueFamilyIndices indices = findQueueFamilies(gpu);
	const uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapchainDetails.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; //Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	//Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain);

	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear swapchain.");

	//Obtener el número final de imágenes del swapchain.
	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);

	//Obtener las imágenes del swapchain.
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());

	//Actualizar las variables de la clase.
	swapchainFormat = surfaceFormat.format;
	swapchainExtent = extent;
}


void RenderAPI::createSwapchainImageViews() {
	swapchainImageViews.resize(swapchainImages.size());

	//Por cada imagen...
	for (uint32_t i = 0; i < swapchainImageViews.size(); i++) {
		//Create info.
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapchainImageViews[i]);
	}
}


void RenderAPI::createRenderpass() {
	screenRenderpass = new Renderpass(logicalDevice);

	RenderpassAttachment clrAttachment{};
	clrAttachment.AddAttachment(swapchainFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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
	dep.vulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.vulkanDependency.dstSubpass = 0;
	dep.vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep.vulkanDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep.vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.vulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep.vulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	SubpassDependency dep2;
	dep2.vulkanDependency.srcSubpass = 0;
	dep2.vulkanDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	dep2.vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep2.vulkanDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep2.vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep2.vulkanDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep2.vulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	sbPass.AddDependency(dep);
	sbPass.AddDependency(dep2);

	screenRenderpass->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
	screenRenderpass->AddSubpass(sbPass);
	screenRenderpass->AddAttachment(clrAttachment);
	screenRenderpass->AddAttachment(dpthAttachment);
	screenRenderpass->Create();
}

void RenderAPI::createCommandPool() {
	QueueFamilyIndices indices = findQueueFamilies(gpu);

	//Para la graphics q.
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear command pool.");
}


void RenderAPI::createDepthResources() {
	VkFormat depthFormat = getDepthFormat();
	VULKAN::VulkanImageGen::CreateImage(&depthImage, { renderTargetSizeX, renderTargetSizeY }, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
}


void RenderAPI::createFramebuffers() {
	framebuffers.resize(swapchainImageViews.size());

	//Crear los framebuffers.
	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		Framebuffer* framebuffer = new Framebuffer(this);

		framebuffer->AddImageView(swapchainImageViews[i]);
		framebuffer->AddImageView(&depthImage);
		
		framebuffer->Create(screenRenderpass, swapchainExtent.width, swapchainExtent.height);
		
		framebuffers[i] = framebuffer;
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

	VkResult result = vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &globalImageSampler);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear sampler.");
}


void RenderAPI::createDefaultUniformBuffers() {
	VkDeviceSize size = sizeof(UBO);
	uniformBuffers.resize(swapchainImages.size());

	for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i] = CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uniformBuffers[i].Allocate(size);
	}
}


void RenderAPI::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapchainImages.size(), nullptr);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult result;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [0].");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [1].");

		result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [2].");
	}

	fences = new VkFence[swapchainImages.size()];
	for (uint32_t i = 0; i < swapchainImages.size(); i++) {
		vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fences[i]);
	}
}


void RenderAPI::createCommandBuffers() {
	commandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	VkResult result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear command buffers.");
}


void RenderAPI::closeSwapchain() {
	depthImage.Destroy();

	for (auto& i : framebuffers)
		delete i;
	framebuffers.clear();

	for (const auto& i : swapchainImageViews)
		vkDestroyImageView(logicalDevice, i, nullptr);

	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}


void RenderAPI::Close() {
	closeSwapchain();
	ClosePostProcessing();

	delete content;

	delete renderTargetBeforePostProcessing;

	delete materialSystem;

	vkDestroySampler(logicalDevice, globalImageSampler, nullptr);

	for (auto& i : uniformBuffers)
		i.Free();

	Sprite::indexBuffer.Free();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
		vkDestroyFence(logicalDevice, fences[i], nullptr);
	}

	vkDestroyDevice(logicalDevice, nullptr);
#ifdef OSK_DEBUG
	if (checkValidationLayers())
		DestroyDebugUtilsMessengerEXT(instance, debugConsole, nullptr);
#endif
	vkDestroySurfaceKHR(instance, surface, nullptr);

	//Destruir la instancia.
	vkDestroyInstance(instance, nullptr);
}


void RenderAPI::RecreateSwapchain() {
	int32_t width = 0;
	int32_t height = 0;
	glfwGetFramebufferSize(window->window, &width, &height);

	//Si está minimizado, esperar.
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window->window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);

	closeSwapchain();

	renderTargetSizeX = window->GetSize().X * renderResolutionMultiplier;
	renderTargetSizeY = window->GetSize().Y * renderResolutionMultiplier;

	createSwapchain();
	createSwapchainImageViews();
	createRenderpass();
	
	createDepthResources();

	createFramebuffers();

	createRenderTarget();
	RecreatePostProcessing();
}


void RenderAPI::createRenderTarget() {
	renderTargetSizeX = window->GetSize().X * renderResolutionMultiplier;
	renderTargetSizeY = window->GetSize().Y * renderResolutionMultiplier;

	renderTargetBeforePostProcessing->Clear(false);

	renderTargetBeforePostProcessing->SetFormat(swapchainFormat);
	renderTargetBeforePostProcessing->SetSize(renderTargetSizeX, renderTargetSizeY, true);

	VkImageView views[] = { renderTargetBeforePostProcessing->renderedSprite.texture->image.view, depthImage.view };
	renderTargetBeforePostProcessing->CreateFramebuffers(swapchainImages.size() + 1, views, 2);

	Logger::DebugLog("Recreated swapchain.");
	Logger::DebugLog("Resolution multiplier = " + std::to_string(renderResolutionMultiplier) + ".");
	Logger::DebugLog("Renderer resolution = " + ToString(Vector2ui(renderTargetSizeX, renderTargetSizeY)) + ".");
	Logger::DebugLog("Output resolution = " + ToString(window->GetSize().ToVector2ui()) + ".");

	renderTargetBeforePostProcessing->renderedSprite.transform.SetPosition({ 0.0f });
	renderTargetBeforePostProcessing->renderedSprite.transform.SetScale(window->GetSize().ToVector2f());//Vector2ui{ renderTargetSizeX, renderTargetSizeY }.ToVector2f() / renderResolutionMultiplier
}

GPUDataBuffer RenderAPI::CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const {
	GPUDataBuffer buffer;
	buffer.Create(logicalDevice, usage, prop, memoryProperties);

	return buffer;
}

void RenderAPI::CreateDynamicUBO(GPUDataBuffer& buffer, VkDeviceSize sizeOfStruct, uint32_t numberOfInstances) const {
	size_t minAlignment = gpuInfo.minAlignment;
	size_t alignment = sizeOfStruct;
	if (minAlignment > 0) {
		alignment = (alignment + minAlignment - 1) & ~(minAlignment - 1);
	}

	size_t bufferSize = alignment * numberOfInstances;

	buffer = CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	buffer.alignment = alignment;
	buffer.Allocate(bufferSize);
	buffer.SetDynamicUboStructureSize(sizeOfStruct);
}


/*void RenderAPI::SetRenderizableScene(RenderizableScene* scene) {
	Scene = scene;
	Scene->TargetRenderpass = RTarget->VRenderpass;
}*/

void RenderAPI::AddStage(RenderStage* stage) {
	stages.push_back(stage);
}

void RenderAPI::RemoveStage(RenderStage* stage) {
	stages.remove(stage);
}

void RenderAPI::AddSingleTimeStage(RenderStage* stage) {
	singleTimeStages.push_back(stage);
}

void RenderAPI::DrawStage(RenderStage* stage, VkCommandBuffer cmdBuffer, uint32_t iteration) {
	if (!stage->renderTarget)
		return;
	
	if (stage->scene != nullptr) {
		stage->scene->UpdateLightsBuffers();

		stage->scene->DrawShadows(cmdBuffer, iteration);

		//for (auto& cubeMap : stage->Scene->cubeShadowMaps)
			//stage->Scene->DrawPointShadows(cmdBuffer, iteration, cubeMap);
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = stage->renderTarget->vulkanRenderpass->vulkanRenderpass;
	renderPassInfo.framebuffer = stage->renderTarget->targetFramebuffers[iteration]->framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };

	renderPassInfo.renderArea.extent = { stage->renderTarget->size.X, stage->renderTarget->size.Y };
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
	clearValues[1] = { 1.0f, 0.0f }; //Depth.
	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	stage->renderTarget->TransitionToRenderTarget(&cmdBuffer);
	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	SetViewport(cmdBuffer);

	if (stage->scene != nullptr) {
		stage->scene->Draw(cmdBuffer, iteration);
	}

	GraphicsPipeline* pipeline = GetMaterialSystem()->GetMaterial(defaultMaterial2D_Name)->GetGraphicsPipeline(stage->renderTarget->vulkanRenderpass);

	for (auto& spriteBatch : stage->spriteBatches) {
		
		if (!spriteBatch->spritesToDraw.IsEmpty()) {
			
			pipeline->Bind(cmdBuffer);
			vkCmdBindIndexBuffer(cmdBuffer, Sprite::indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = Sprite::indices.size();

			for (auto& sprite : spriteBatch->spritesToDraw) {
				VkBuffer vertexBuffers[] = { sprite.vertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

				sprite.spriteMaterial->GetDescriptorSet()->Bind(cmdBuffer, pipeline, iteration);

				PushConst2D pConst = sprite.pushConst;
				vkCmdPushConstants(cmdBuffer, pipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
			}
		}
	}

	vkCmdEndRenderPass(cmdBuffer);
	stage->renderTarget->TransitionToTexture(&cmdBuffer);
}

void RenderAPI::updateCommandBuffers() {
	updateCmdProfilingUnit.Start();

	/*for (auto& spriteBatch : Stage.SpriteBatches) {
		for (auto& i : spriteBatch->spritesToDraw) {
			if (i.hasChanged)
				updateSpriteVertexBuffer(i);
		}
	}*/

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		vkResetCommandBuffer(commandBuffers[i], 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar command buffer.");

		for (auto& stage : singleTimeStages)
			DrawStage(stage, commandBuffers[i], i);

		for (auto& stage : stages)
			DrawStage(stage, commandBuffers[i], i);
				
		if (renderSystem) {
			renderSystem->OnDraw(commandBuffers[i], i);
		}

		postProcessingSettings.screenSizeX = (int)window->GetSize().X;
		postProcessingSettings.screenSizeY = (int)window->GetSize().Y;
		postProcessingSettings.resolutionMultiplier = renderResolutionMultiplier;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = screenRenderpass->vulkanRenderpass;
		renderPassInfo.framebuffer = framebuffers[i]->framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = swapchainExtent;
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		//Comenzar el renderizado.
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		SetViewport(commandBuffers[i], -(int32_t)window->GetSize().X, -(int32_t)window->GetSize().Y, (uint32_t)window->GetSize().X * 2, (uint32_t)window->GetSize().Y * 2);

		screenGraphicsPipeline->Bind(commandBuffers[i]);
		vkCmdBindIndexBuffer(commandBuffers[i], Sprite::indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		const size_t indicesSize = Sprite::indices.size();

		VkBuffer vertexBuffers[] = { renderTargetBeforePostProcessing->renderedSprite.vertexBuffer.buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

		screenDescriptorSet->Bind(commandBuffers[i], screenGraphicsPipeline, i);

		vkCmdPushConstants(commandBuffers[i], screenGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PostProcessingSettings_t), &postProcessingSettings);
		vkCmdDrawIndexed(commandBuffers[i], indicesSize, 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		result = vkEndCommandBuffer(commandBuffers[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar renderpass.");

		updateCmdProfilingUnit.End();
	}

	singleTimeStages.clear();
}

void RenderAPI::createSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->vertices.size();

	sprite->vertexBuffer = CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	sprite->vertexBuffer.Allocate(size);
	
	updateSpriteVertexBuffer(sprite);
}

void RenderAPI::createSpriteIndexBuffer() const {
	VkDeviceSize size = sizeof(Sprite::indices[0]) * Sprite::indices.size();

	GPUDataBuffer stagingBuffer = CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.Allocate(size);

	stagingBuffer.Write(Sprite::indices.data(), size);

	Sprite::indexBuffer = CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	Sprite::indexBuffer.Allocate(size);
	CopyBuffer(stagingBuffer, Sprite::indexBuffer, size);

	stagingBuffer.Free();
}


void RenderAPI::updateSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->vertices.size();

	void* data;
	vkMapMemory(logicalDevice, sprite->vertexBuffer.memory, 0, size, 0, &data);
	memcpy(data, sprite->vertices.data(), (size_t)size);
	vkUnmapMemory(logicalDevice, sprite->vertexBuffer.memory);

	sprite->hasChanged = false;
}

void RenderAPI::updateSpriteVertexBuffer(SpriteContainer& sprite) const {
	VkDeviceSize size = sizeof(Vertex) * 4;

	void* data;
	vkMapMemory(logicalDevice, sprite.vertexMemory, 0, size, 0, &data);
	memcpy(data, sprite.vertices, (size_t)size);
	vkUnmapMemory(logicalDevice, sprite.vertexMemory);

	sprite.hasChanged = false;
}


//Copia el contenido de un buffer a otro buffer.
void RenderAPI::CopyBuffer(GPUDataBuffer& source, GPUDataBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = sourceOffset;
	copyRegion.dstOffset = destinationOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, source.buffer, destination.buffer, 1, &copyRegion);

	endSingleTimeCommandBuffer(cmdBuffer);
}


VkCommandBuffer RenderAPI::beginSingleTimeCommandBuffer() const {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

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

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &cmdBuffer);
}


//Obtiene el tipo de memoria indicado.
uint32_t RenderAPI::getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const {
	//Tipos de memoria disponibles.
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		if (memoryTypeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;

	return -1;

	throw std::runtime_error("failed to find suitable memory type!");
}


VkFormat RenderAPI::getDepthFormat() const {
	return getSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

GPUinfo RenderAPI::getGPUinfo(VkPhysicalDevice gpu) const {
	GPUinfo info{};
	info.gpu = gpu;
	//Obtiene las propiedades de la gpu.
	vkGetPhysicalDeviceProperties(gpu, &info.properties);
	//Obtiene las características de la GPU.
	vkGetPhysicalDeviceFeatures(gpu, &info.features);

	//Comprobar soporte de colas.
	info.families = findQueueFamilies(gpu);

	//Comprobar soporte de swapchain.
	bool swapchainSupported = false;

	info.shapchainSupport = getSwapchainSupportDetails(gpu);
	swapchainSupported = !info.shapchainSupport.presentModes.empty() && !info.shapchainSupport.formats.empty();

	info.isSuitable = info.families.IsComplete() && checkGPUextensionSupport(gpu) && swapchainSupported && info.features.samplerAnisotropy;
	info.minAlignment = info.properties.limits.minUniformBufferOffsetAlignment;

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
	glfwGetFramebufferSize(window->window, &width, &height);

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
		vkGetPhysicalDeviceFormatProperties(gpu, format, &properties);

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
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details.surfaceCapabilities);

	//Número de formatos soportados.
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

	//Obtener formatos soportados.
	details.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details.formats.data());

	//Números de modos de presentación.
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);

	//Obtener modos de presentación.
	details.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &formatCount, details.presentModes.data());

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
			indices.graphicsFamily = i;

		//Soporte para presentación.
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

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
	finalRenderTarget = CreateNewRenderTarget();
	finalRenderTarget->SetFormat(swapchainFormat);
	finalRenderTarget->SetSize((uint32_t)window->GetSize().X, (uint32_t)window->GetSize().Y);
	finalRenderTarget->CreateSprite(content);

	VULKAN::VulkanImageGen::CreateImage(&finalRenderTarget->renderedSprite.texture->image, finalRenderTarget->size, swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&finalRenderTarget->renderedSprite.texture->image, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(finalRenderTarget->renderedSprite.texture->image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	VULKAN::VulkanImageGen::TransitionImageLayout(&finalRenderTarget->renderedSprite.texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 0);

	VULKAN::VulkanImageGen::CreateImageSampler(finalRenderTarget->renderedSprite.texture->image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

	screenDescriptorPool = CreateNewDescriptorPool();
	screenDescriptorPool->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	screenDescriptorPool->Create(1);

	screenDescriptorLayout = CreateNewDescriptorLayout();
	screenDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	screenDescriptorLayout->descriptorPool = screenDescriptorPool;
	screenDescriptorLayout->Create();

	screenDescriptorSet = CreateNewDescriptorSet();
	screenDescriptorSet->Create(screenDescriptorLayout, screenDescriptorPool, true);
	screenDescriptorSet->AddImage(&renderTargetBeforePostProcessing->renderedSprite.texture->image, renderTargetBeforePostProcessing->renderedSprite.texture->image.sampler, 0);
	screenDescriptorSet->Update();

	screenGraphicsPipeline = CreateNewGraphicsPipeline("shaders/VK_Post/vert.spv", "shaders/VK_Post/frag.spv");
	screenGraphicsPipeline->SetViewport({ -(float)swapchainExtent.width, -(float)swapchainExtent.height, (float)swapchainExtent.width * 2, (float)swapchainExtent.height * 2 });
	screenGraphicsPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	screenGraphicsPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	screenGraphicsPipeline->SetDepthStencil(false);
	screenGraphicsPipeline->SetPushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PostProcessingSettings_t));
	screenGraphicsPipeline->SetLayout(&screenDescriptorLayout->vulkanDescriptorSetLayout);
	screenGraphicsPipeline->Create(screenRenderpass);

	finalRenderTarget->vulkanRenderpass = screenRenderpass;
	VkImageView views[] = { finalRenderTarget->renderedSprite.texture->image.view, depthImage.view };
	finalRenderTarget->CreateFramebuffers(4, views, 2);
	finalRenderTarget->renderedSprite.transform.SetScale(window->GetSize().ToVector2f() * renderResolutionMultiplier);
	updateSpriteVertexBuffer(&renderTargetBeforePostProcessing->renderedSprite);
}

void RenderAPI::RecreatePostProcessing() {
	ClosePostProcessing();
	InitPostProcessing();
}

void RenderAPI::ClosePostProcessing() {
	SafeDelete(&screenDescriptorLayout);
	SafeDelete(&screenDescriptorSet);
	SafeDelete(&screenGraphicsPipeline);
	SafeDelete(&finalRenderTarget);
}