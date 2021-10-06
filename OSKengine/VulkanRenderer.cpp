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
#include "MaterialSlot.h"

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
	viewport.x = (float)x;
	viewport.y = (float)y;
	if (sizeX != 0)
		viewport.width = (float)sizeX;
	else
		viewport.width = (float)window->GetSize().X;
	if (sizeY != 0)
		viewport.height = (float)sizeY;
	else
		viewport.height = (float)window->GetSize().Y;
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
	getGpu();
	createLogicalDevice();

	memoryAllocator = new MemoryAllocator();
	memoryAllocator->Init(logicalDevice, memoryProperties);

	swapchain = new Swapchain(this);
	swapchain->Create();

	materialSystem = new MaterialSystem(this);

	renderTargetSizeX = (uint32_t)(swapchain->size.width * renderResolutionMultiplier);
	renderTargetSizeY = (uint32_t)(swapchain->size.height * renderResolutionMultiplier);

	createCommandPool();
	createGlobalImageSampler();
	createDefaultUniformBuffers();
	ContentManager::DefaultTexture = content->LoadTexture(ContentManager::DEFAULT_TEXTURE_PATH);

	//MATERIALS
	{
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.cullMode = PolygonCullMode::NONE;
		pipelineInfo.vertexPath = Settings.vertexShaderPath2D;
		pipelineInfo.fragmentPath = Settings.fragmentShaderPath2D;
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst2D) });
		materialSystem->GetMaterial(MPIPE_2D)->SetPipelineSettings(pipelineInfo);
	}
	{
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.vertexPath = Settings.vertexShaderPath3D;
		pipelineInfo.fragmentPath = Settings.fragmentShaderPath3D;
		pipelineInfo.useDepthStencil = true;
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst3D) });
		materialSystem->GetMaterial(MPIPE_3D)->SetPipelineSettings(pipelineInfo);
	}
	{
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.cullMode = PolygonCullMode::FRONT;
		pipelineInfo.frontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;
		pipelineInfo.vertexPath = Settings.skyboxVertexPath;
		pipelineInfo.fragmentPath = Settings.skyboxFragmentPath;
		pipelineInfo.useDepthStencil = false;
		materialSystem->GetMaterial(MPIPE_SKYBOX)->SetPipelineSettings(pipelineInfo);
	}
	{
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.polygonMode = PolygonMode::FILL;
		pipelineInfo.cullMode = PolygonCullMode::FRONT;
		pipelineInfo.frontFaceType = PolygonFrontFaceType::CLOCKWISE;
		pipelineInfo.vertexPath = "shaders/VK_Shadows/vert.spv";
		pipelineInfo.fragmentPath = "shaders/VK_Shadows/frag.spv";
		pipelineInfo.useDepthStencil = true;
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::VERTEX, sizeof(PushConst3D) });
		materialSystem->GetMaterial(MPIPE_SHADOWS3D)->SetPipelineSettings(pipelineInfo);
	} 
	{
		MaterialPipelineCreateInfo pipelineInfo{};
		pipelineInfo.polygonMode = PolygonMode::FILL;
		pipelineInfo.cullMode = PolygonCullMode::NONE;
		pipelineInfo.frontFaceType = PolygonFrontFaceType::CLOCKWISE;
		pipelineInfo.vertexPath = "shaders/VK_Post/vert.spv";
		pipelineInfo.fragmentPath = "shaders/VK_Post/frag.spv";
		pipelineInfo.pushConstants.push_back({ MaterialBindingShaderStage::FRAGMENT, sizeof(PostProcessingSettings_t) });
		materialSystem->GetMaterial(MPIPE_POSTPROCESS)->SetPipelineSettings(pipelineInfo);
	}
	//!MATERIALS

	createSyncObjects();
	createCommandBuffers();

	createSpriteIndexBuffer();

	Skybox::Model = content->LoadModelData("models/Skybox/cube.obj");

	defaultCamera2D = Camera2D(window);
	CreateCamera();

	hasBeenInit = true;

	content->LoadSprite(&OskEngineIconSprite, "Resources/OSKengine_icon_lowres_48.png");
	content->LoadSprite(&OskIconSprite, "Resources/OSK_icon_lowres.png");

	renderTargetSizeX = (uint32_t)(window->GetSize().X * renderResolutionMultiplier);
	renderTargetSizeY = (uint32_t)(window->GetSize().Y * renderResolutionMultiplier);

	renderTargetBeforePostProcessing->SetSize(renderTargetSizeX, renderTargetSizeY);
	renderTargetBeforePostProcessing->CreateSprite(content.GetPointer());
	renderTargetBeforePostProcessing->CreateRenderpass(gpuInfo.maxMsaaSamples);

	renderTargetBeforePostProcessing->renderedSprite.transform.SetPosition({ 0.0f });
	renderTargetBeforePostProcessing->renderedSprite.transform.SetScale(window->GetSize().ToVector2f());


	finalRenderTarget = CreateNewRenderTarget().GetPointer();
	finalRenderTarget->SetSwapchain(swapchain->imageViews);
	finalRenderTarget->SetSize(renderTargetSizeX, renderTargetSizeY);
	finalRenderTarget->CreateSprite(content.GetPointer());
	finalRenderTarget->CreateRenderpass(gpuInfo.maxMsaaSamples);

	finalRenderTarget->renderedSprite.transform.SetPosition({ 0.0f });
	finalRenderTarget->renderedSprite.transform.SetScale(window->GetSize().ToVector2f());

	InitPostProcessing();
}

MemoryAllocator* RenderAPI::GetGpuMemoryAllocator() const {
	return memoryAllocator.GetPointer();
}

VkSampleCountFlagBits RenderAPI::GetMsaaSamples() const {
	return gpuInfo.maxMsaaSamples;
}

OwnedPtr<Renderpass> RenderAPI::CreateNewRenderpass() {
	return new Renderpass(logicalDevice);
}

OwnedPtr<RenderTarget> RenderAPI::CreateNewRenderTarget() {
	return new RenderTarget(this);
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

OwnedPtr<Framebuffer> RenderAPI::CreateNewFramebuffer() {
	return new Framebuffer(this);
}

Camera3D* RenderAPI::CreateCamera() {
	cameras.push_back({ 0, 0, 0 });

	auto& cam = cameras.back();
	cam.window = window;

	VkDeviceSize size = sizeof(UboCamera3D);
	cam.GetUniformBuffer().GetBuffersRef().resize(swapchain->GetImageCount());

	for (uint32_t i = 0; i < cam.GetUniformBuffer().GetBuffersRef().size(); i++) {
		cam.GetUniformBuffer().GetBuffersRef()[i] = new GpuDataBuffer;
		AllocateBuffer(cam.GetUniformBuffer().GetBuffers()[i].GetPointer(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	return &cameras.back();
}

Camera3D* RenderAPI::GetDefaultCamera() {
	return &cameras.front();
}

void RenderAPI::RemoveCamera(Camera3D* camera) {
	auto it = cameras.begin();
	for (size_t i = 0; i < cameras.size(); i++) {
		if (&it.operator*() == camera) {
			cameras.erase(it);

			return;
		}

		++it;
	}
}

void RenderAPI::RenderFrame() {
	renderProfilingUnit.Start();

	const double startTime = glfwGetTime();

	{
		defaultCamera2D.Update();

		for (auto& cam : cameras) {
			for (size_t i = 0; i < cam.GetUniformBuffer().GetBuffers().size(); i++) {
				void* data;
				vkMapMemory(logicalDevice, cam.GetUniformBuffer().GetBuffers()[i]->memorySubblock->memory, cam.GetUniformBuffer().GetBuffers()[i]->memorySubblock->GetOffset(), sizeof(UboCamera3D), 0, &data);
				UboCamera3D ubo{};
				ubo.view = cam.GetView();
				ubo.projection = cam.GetProjection();
				ubo.cameraPos = cam.GetTransform()->GetPosition().ToGLM();

				memcpy(data, &ubo, sizeof(UboCamera3D));
				vkUnmapMemory(logicalDevice, cam.GetUniformBuffer().GetBuffers()[i]->memorySubblock->memory);
			}
		}

		updateCommandBuffers();
	}
	//Repreenta cual es la imagen que se va a renderizar.
	uint32_t nextImageIndex = 0;

	vkWaitForFences(logicalDevice, 1, &fences[renderVars.currentImage], VK_TRUE, UINT64_MAX);
	//Adquiere la siguiente imágen sobre la que se va a renderizar.
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain->swapchain, UINT64_MAX, imageAvailableSemaphores[0], VK_NULL_HANDLE, &nextImageIndex);
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
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
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
	VkSwapchainKHR swapChains[] = { swapchain->swapchain };
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

	const double endTime = glfwGetTime();

	const double targetMS = 1 / fpsLimit;
	renderProfilingUnit.End();
	//if ((endTime - startTime) < targetMS)
		//std::this_thread::sleep_for(std::chrono::milliseconds((long)(targetMS * 1000 - (endTime - startTime) * 1000)));
}

SpriteBatch RenderAPI::CreateSpriteBatch() {
	SpriteBatch spriteBatch{};
	spriteBatch.renderer = this;

	return spriteBatch;
}


OwnedPtr<OSK::GraphicsPipeline> RenderAPI::CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	return new GraphicsPipeline(logicalDevice, vertexPath, fragmentPath);
}

OwnedPtr<DescriptorPool> RenderAPI::CreateNewDescriptorPool() const {
	return new DescriptorPool(logicalDevice, swapchain->GetImageCount());
}

OwnedPtr < DescriptorLayout> RenderAPI::CreateNewDescriptorLayout(uint32_t set) const {
	return new DescriptorLayout(logicalDevice, set);
}

OwnedPtr < DescriptorSet> RenderAPI::CreateNewDescriptorSet() const {
	return new DescriptorSet(logicalDevice, swapchain->GetImageCount());
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
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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
	VkResult result = glfwCreateWindowSurface(instance, window->window.GetPointer(), nullptr, &surface);

	//Error-handling.
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear superficie.");
}


void RenderAPI::getGpu() {
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
	std::vector<GpuInfo> gpus{};
	for (const auto& gpu : devices)
		gpus.push_back(getGpuInfo(gpu));

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
	
}


void RenderAPI::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapchain->GetImageCount(), nullptr);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [0].");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [1].");

		result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear semáforo [2].");
	}

	fences = new VkFence[swapchain->GetImageCount()];
	for (uint32_t i = 0; i < swapchain->GetImageCount(); i++) {
		vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fences[i]);
	}
}


void RenderAPI::createCommandBuffers() {
	commandBuffers.resize(swapchain->GetImageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	VkResult result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear command buffers.");
}


void RenderAPI::Close() {
	swapchain.Delete();

	for (auto& i : cameras)
		i.GetUniformBuffer().GetBuffersRef().clear();

	screenDescriptorLayout.Delete();
	screenDescriptorSet.Delete();
	screenGraphicsPipeline.Delete();
	finalRenderTarget.Delete();
	screenDescriptorPool.Delete();

	content.Delete();
	renderTargetBeforePostProcessing.Delete();
	finalRenderTarget.Delete();
	materialSystem.Delete();

	vkDestroySampler(logicalDevice, globalImageSampler, nullptr);

	Sprite::indexBuffer.~SharedPtr();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
		vkDestroyFence(logicalDevice, fences[i], nullptr);
	}

#ifdef OSK_DEBUG
	if (checkValidationLayers())
		DestroyDebugUtilsMessengerEXT(instance, debugConsole, nullptr);
#endif
	vkDestroySurfaceKHR(instance, surface, nullptr);

	memoryAllocator.Delete();

	vkDestroyDevice(logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}


void RenderAPI::RecreateSwapchain() {
	int32_t width = 0;
	int32_t height = 0;
	glfwGetFramebufferSize(window->window.GetPointer(), &width, &height);

	//Si está minimizado, esperar.
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window->window.GetPointer(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);

	swapchain->Close();

	renderTargetSizeX = (uint32_t)(window->GetSize().X * renderResolutionMultiplier);
	renderTargetSizeY = (uint32_t)(window->GetSize().Y * renderResolutionMultiplier);

	swapchain->Create();

	recreateRenderTargets();
	RecreatePostProcessing();
}


void RenderAPI::recreateRenderTargets() {
	renderTargetSizeX = (uint32_t)(window->GetSize().X * renderResolutionMultiplier);
	renderTargetSizeY = (uint32_t)(window->GetSize().Y * renderResolutionMultiplier);

	renderTargetBeforePostProcessing->Resize(renderTargetSizeX, renderTargetSizeY);

	renderTargetBeforePostProcessing->renderedSprite.transform.SetPosition({ 0.0f });
	renderTargetBeforePostProcessing->renderedSprite.transform.SetScale(window->GetSize().ToVector2f());


	finalRenderTarget->Resize(renderTargetSizeX, renderTargetSizeY);

	finalRenderTarget->renderedSprite.transform.SetPosition({ 0.0f });
	finalRenderTarget->renderedSprite.transform.SetScale(window->GetSize().ToVector2f());
}

SharedPtr<GpuDataBuffer> RenderAPI::CreateDynamicUBO(VkDeviceSize sizeOfStruct, uint32_t numberOfInstances) {
	SharedPtr<GpuDataBuffer> buffer = new GpuDataBuffer();

	size_t minAlignment = gpuInfo.minAlignment;
	size_t alignment = sizeOfStruct;
	if (minAlignment > 0)
		alignment = (alignment + minAlignment - 1) & ~(minAlignment - 1);

	size_t bufferSize = alignment * numberOfInstances;

	buffer = new GpuDataBuffer;
	buffer->alignment = (uint32_t)alignment;
	AllocateBuffer(buffer.GetPointer(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	buffer->SetDynamicUboStructureSize(sizeOfStruct);

	return buffer;
}


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

	stage->renderTarget->BeginRenderpass(cmdBuffer, iteration);

	SetViewport(cmdBuffer);

	if (stage->scene != nullptr)
		stage->scene->Draw(cmdBuffer, iteration, stage->renderTarget);

	GraphicsPipeline* pipeline = GetMaterialSystem()->GetMaterial(MPIPE_2D)->GetGraphicsPipeline(stage->renderTarget->renderpass.GetPointer());

	for (auto& spriteBatch : stage->spriteBatches) {
		
		if (!spriteBatch->spritesToDraw.IsEmpty()) {
			
			pipeline->Bind(cmdBuffer);
			vkCmdBindIndexBuffer(cmdBuffer, Sprite::indexBuffer->memorySubblock->vkBuffer, 0, VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = (uint32_t)Sprite::indices.size();

			for (auto& sprite : spriteBatch->spritesToDraw) {
				VkBuffer vertexBuffers[] = { sprite.vertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

				sprite.spriteMaterial->GetMaterialSlot(MSLOT_TEXTURE_2D)->GetDescriptorSet()->Bind(cmdBuffer, pipeline, iteration);

				PushConst2D pConst = sprite.pushConst;
				vkCmdPushConstants(cmdBuffer, pipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
			}
		}
	}

	stage->renderTarget->EndRenderpass();
}

void RenderAPI::updateCommandBuffers() {
	updateCmdProfilingUnit.Start();

	for (uint32_t i = 0; i < (uint32_t)commandBuffers.size(); i++) {
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

		finalRenderTarget->BeginRenderpass(commandBuffers[i], i);

		SetViewport(commandBuffers[i], -(int32_t)window->GetSize().X, -(int32_t)window->GetSize().Y, (uint32_t)window->GetSize().X * 2, (uint32_t)window->GetSize().Y * 2);

		screenGraphicsPipeline->Bind(commandBuffers[i]);
		vkCmdBindIndexBuffer(commandBuffers[i], Sprite::indexBuffer->memorySubblock->vkBuffer, Sprite::indexBuffer->memorySubblock->GetOffset(), VK_INDEX_TYPE_UINT16);
		const size_t indicesSize = Sprite::indices.size();

		VkBuffer vertexBuffers[] = { renderTargetBeforePostProcessing->renderedSprite.vertexBuffer->memorySubblock->vkBuffer };
		VkDeviceSize offsets[] = { renderTargetBeforePostProcessing->renderedSprite.vertexBuffer->memorySubblock->GetOffset() };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

		screenDescriptorSet->Bind(commandBuffers[i], screenGraphicsPipeline.GetPointer(), i);

		vkCmdPushConstants(commandBuffers[i], screenGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PostProcessingSettings_t), &postProcessingSettings);
		vkCmdDrawIndexed(commandBuffers[i], indicesSize, 1, 0, 0, 0);

		finalRenderTarget->EndRenderpass();

		result = vkEndCommandBuffer(commandBuffers[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar renderpass.");

		updateCmdProfilingUnit.End();
	}

	singleTimeStages.clear();
}

void RenderAPI::createSpriteVertexBuffer(Sprite* sprite) {
	VkDeviceSize size = sizeof(Vertex) * sprite->vertices.size();

	sprite->vertexBuffer = new GpuDataBuffer;
	AllocateBuffer(sprite->vertexBuffer.GetPointer(), size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	updateSpriteVertexBuffer(sprite);
}

void RenderAPI::createSpriteIndexBuffer() {
	VkDeviceSize size = sizeof(Sprite::indices[0]) * Sprite::indices.size();

	SharedPtr<GpuDataBuffer> stagingBuffer = new GpuDataBuffer;
	AllocateBuffer(stagingBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer->Write(Sprite::indices.data(), size);

	Sprite::indexBuffer = new GpuDataBuffer;
	AllocateBuffer(Sprite::indexBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	CopyBuffer(stagingBuffer.Get(), Sprite::indexBuffer.Get(), size);
}


void RenderAPI::updateSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->vertices.size();

	sprite->vertexBuffer->Write(sprite->vertices.data(), size);

	sprite->hasChanged = false;
}

void RenderAPI::updateSpriteVertexBuffer(SpriteContainer* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * 4;

	void* data;
	vkMapMemory(logicalDevice, sprite->vertexMemory, sprite->bufferOffset, size, 0, &data);
	memcpy(data, sprite->vertices, (size_t)size);
	vkUnmapMemory(logicalDevice, sprite->vertexMemory);

	sprite->hasChanged = false;
}


//Copia el contenido de un buffer a otro buffer.
void RenderAPI::CopyBuffer(GpuDataBuffer& source, GpuDataBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = sourceOffset + source.memorySubblock->totalOffsetFromBlock;
	copyRegion.dstOffset = destinationOffset + destination.memorySubblock->totalOffsetFromBlock;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, source.memorySubblock->vkBuffer, destination.memorySubblock->vkBuffer, 1, &copyRegion);

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
uint32_t RenderAPI::getMemoryType(uint32_t memoryTypeFilter, VkMemoryPropertyFlags flags) const {
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

GpuInfo RenderAPI::getGpuInfo(VkPhysicalDevice gpu) const {
	GpuInfo info{};
	info.renderer = this;

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

	info.SetMaxMsaa();

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

	LogMessageLevels level = LogMessageLevels::WARNING;

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
	finalRenderTarget->Resize(window->GetSize().X, window->GetSize().Y);

	screenDescriptorPool = CreateNewDescriptorPool().GetPointer();
	screenDescriptorPool->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	screenDescriptorPool->Create(1);

	screenDescriptorLayout = CreateNewDescriptorLayout().GetPointer();
	screenDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	screenDescriptorLayout->Create();

	screenDescriptorSet = CreateNewDescriptorSet().GetPointer();
	screenDescriptorSet->Create(screenDescriptorLayout.GetPointer(), screenDescriptorPool.GetPointer(), true);
	screenDescriptorSet->AddImage(renderTargetBeforePostProcessing->renderedSprite.texture->image.GetPointer(), renderTargetBeforePostProcessing->renderedSprite.texture->image->sampler, 0);
	screenDescriptorSet->Update();

	screenGraphicsPipeline = CreateNewGraphicsPipeline("shaders/VK_Post/vert.spv", "shaders/VK_Post/frag.spv").GetPointer();
	screenGraphicsPipeline->SetViewport({ -(float)swapchain->size.width, -(float)swapchain->size.height, (float)swapchain->size.width * 2, (float)swapchain->size.height * 2 });
	screenGraphicsPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	screenGraphicsPipeline->SetMSAA(VK_FALSE, GetMsaaSamples());
	screenGraphicsPipeline->SetDepthStencil(false);
	screenGraphicsPipeline->SetPushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PostProcessingSettings_t));
	screenGraphicsPipeline->SetLayout({ screenDescriptorLayout->vulkanDescriptorSetLayout });
	screenGraphicsPipeline->Create(finalRenderTarget->renderpass.GetPointer());

	finalRenderTarget->renderedSprite.transform.SetScale(window->GetSize().ToVector2f() * renderResolutionMultiplier);
	updateSpriteVertexBuffer(&renderTargetBeforePostProcessing->renderedSprite);
}

void RenderAPI::RecreatePostProcessing() {
	ClosePostProcessing();
	InitPostProcessing();
}

void RenderAPI::ClosePostProcessing() {
	screenDescriptorLayout.Delete();
	screenDescriptorSet.Delete();
	screenGraphicsPipeline.Delete();
}

void RenderAPI::AllocateBuffer(GpuDataBuffer* buffer, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	OSK_ASSERT(buffer, "buffer es null.");

	memoryAllocator->Allocate(buffer, size, usage, properties);
}