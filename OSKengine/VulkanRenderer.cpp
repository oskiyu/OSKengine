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
#include "BaseUIElement.h"

#include "WindowAPI.h"
#include <gtc/type_ptr.hpp>
#include "Model.h"

#include <ft2build.h>
#include <thread>
#include "Camera3D.h"
#include FT_FREETYPE_H

#include <ktx.h>
#include <ktxvulkan.h>

using namespace OSK;
using namespace OSK::VULKAN;

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;


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

OskResult VulkanRenderer::Init(const RenderMode& mode, const std::string& appName, const Version& gameVersion) {
	renderMode = mode;
	
	lights.Points.resize(16);
	lights.Points.push_back({});
	lights.Points[0].Radius = 50;
	lights.Points[0].Color = OSK::Color(0.0f, 1.0f, 1.0f);
	lights.Points[0].Intensity = 2.0f;
	lights.Points[0].Constant = 1.0f;
	lights.Points[0].Linear = 0.09f;
	lights.Points[0].Quadratic = 0.032f;
	lights.Points[0].Position = { 5, 5, 5 };
	lights.Directional = DirectionalLight{ OSK::Vector3(-0.7f, 0.8f, -0.4f), OSK::Color::RED(), 2.0f };

	createInstance(appName, gameVersion);
	setupDebugConsole();
	createSurface();
	getGPU();
	createLogicalDevice();
	createSwapchain();
	createSwapchainImageViews();
	createRenderpass();
	createDescriptorSetLayout();

	PhongDescriptorLayout = CreateNewPhongDescriptorLayout();
	SkyboxDescriptorLayout = CreateNewSkyboxDescriptorLayout();

	OskResult result = createGraphicsPipeline2D();
	if (result != OskResult::SUCCESS) {
		OSK_SHOW_TRACE();
		return result;
	}
	result = createGraphicsPipeline3D();
	if (result != OskResult::SUCCESS) {
		OSK_SHOW_TRACE();
		return result;
	}

	SkyboxGraphicsPipeline = CreateNewSkyboxPipeline("shaders/VK_Skybox/vert.spv", "shaders/VK_Skybox/frag.spv");

	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createGlobalImageSampler();
	createDefaultUniformBuffers();
	createDescriptorPool();
	createSyncObjects();
	createCommandBuffers();

	Skybox::Model = Content->LoadModelData("models/Skybox/cube.obj");
	//Content->LoadSkybox(LevelSkybox, "skybox/skybox.ktx");

	DefaultCamera2D = Camera2D(Window);
	DefaultCamera3D.Window = Window;

	/*const std::vector<Vertex> Vertices = {
			{{-1, -1, -1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
			{{1, -1, -1}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
			{{1, 1, -1}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
			{{-1, 1, -1}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
	};
	const std::vector<vertexIndex_t> Indices = {
		0, 1, 2, 2, 3, 0
	};*/
	//plane = Content->CreateModel(Vertices, Indices);
	//LoadAnimatedModel(model, "models/anim2/goblin.dae");
	//LoadAnimatedModel(model, "models/anim/boblampclean.md5mesh");

	hasBeenInit = true;

	return OskResult::SUCCESS;
}


void VulkanRenderer::SetPresentMode(const PresentMode& mode) {
	targetPresentMode = mode;

	if (hasBeenInit)
		RecreateSwapchain();
}


PresentMode VulkanRenderer::GetCurrentPresentMode() const {
	return targetPresentMode;
}


void VulkanRenderer::ReloadShaders() {
	RecreateSwapchain();
}


void VulkanRenderer::RenderFrame() {
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

			for (uint32_t i = 0; i < OSK_ANIM_MAX_BONES; i++)
				ubo.bones[i] = glm::mat4(1.0f);

			memcpy(data, &ubo, sizeof(UBO));
			vkUnmapMemory(LogicalDevice, UniformBuffers[i].Memory);
		}

		lights.Points[0].Position = DefaultCamera3D.CameraTransform.GlobalPosition.ToGLM();
		for (auto& i : LightsUniformBuffers)
			lights.UpdateBuffer(LogicalDevice, i);

		if (Settings.AutoUpdateCommandBuffers)
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
	if ((endTime - startTime) < targetMS)
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(targetMS * 1000 - (endTime - startTime) * 1000)));
}


void VulkanRenderer::DrawUserInterface(SpriteBatch& spriteBatch) const {
	if (Window->UserInterface != nullptr && Window->UserInterface->IsActive && Window->UserInterface->IsVisible)
		Window->UserInterface->Draw(spriteBatch);
}


SpriteBatch VulkanRenderer::CreateSpriteBatch() {
	SpriteBatch spriteBatch{};
	spriteBatch.renderer = this;

	return spriteBatch;
}


GraphicsPipeline* VulkanRenderer::CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	return new GraphicsPipeline(LogicalDevice, vertexPath, fragmentPath);
}

DescriptorLayout* VulkanRenderer::CreateNewDescriptorLayout() const {
	return new DescriptorLayout(LogicalDevice, SwapchainImages.size());
}

DescriptorSet* VulkanRenderer::CreateNewDescriptorSet() const {
	return new DescriptorSet(LogicalDevice, SwapchainImages.size());
}

//Phong lighting.

GraphicsPipeline* VulkanRenderer::CreateNewPhongPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	GraphicsPipeline* phongPipeline = CreateNewGraphicsPipeline(vertexPath, fragmentPath);
	phongPipeline->SetViewport({ 0, 0, (float)SwapchainExtent.width, (float)SwapchainExtent.height });
	phongPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	phongPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	phongPipeline->SetDepthStencil(true);
	phongPipeline->SetPushConstants(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConst3D));
	phongPipeline->SetLayout(&PhongDescriptorLayout->VulkanDescriptorSetLayout);
	phongPipeline->Create(renderpass);

	return phongPipeline;
}

DescriptorLayout* VulkanRenderer::CreateNewPhongDescriptorLayout() const {
	DescriptorLayout* descLayout = CreateNewDescriptorLayout();
	descLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	descLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	descLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	descLayout->AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	descLayout->Create(Settings.MaxTextures);

	return descLayout;
}

void VulkanRenderer::CreateNewPhongDescriptorSet(ModelTexture* texture) const {
	texture->PhongDescriptorSet = CreateNewDescriptorSet();
	texture->PhongDescriptorSet->SetDescriptorLayout(PhongDescriptorLayout);
	texture->PhongDescriptorSet->AddUniformBuffers(UniformBuffers, 0, sizeof(UBO));
	texture->PhongDescriptorSet->AddImage(&texture->Albedo, GlobalImageSampler, 1);
	texture->PhongDescriptorSet->AddUniformBuffers(LightsUniformBuffers, 2, lights.Size());
	texture->PhongDescriptorSet->AddImage(&texture->Specular, GlobalImageSampler, 3);
	texture->PhongDescriptorSet->Create();
}

GraphicsPipeline* VulkanRenderer::CreateNewSkyboxPipeline(const std::string& vertexPath, const std::string& fragmentPath) const {
	GraphicsPipeline* skyboxPipeline = CreateNewGraphicsPipeline(vertexPath, fragmentPath);
	skyboxPipeline->SetViewport({ 0, 0, (float)SwapchainExtent.width, (float)SwapchainExtent.height });
	skyboxPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	skyboxPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	skyboxPipeline->SetDepthStencil(false);
	skyboxPipeline->SetLayout(&SkyboxDescriptorLayout->VulkanDescriptorSetLayout);
	skyboxPipeline->Create(renderpass);

	return skyboxPipeline;
}

DescriptorLayout* VulkanRenderer::CreateNewSkyboxDescriptorLayout() const {
	DescriptorLayout* descLayout = CreateNewDescriptorLayout();
	descLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	descLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	descLayout->Create(Settings.MaxTextures);

	return descLayout;
}

void VulkanRenderer::CreateNewSkyboxDescriptorSet(SkyboxTexture* texture) const {
	texture->Descriptor = CreateNewDescriptorSet();
	texture->Descriptor->SetDescriptorLayout(SkyboxDescriptorLayout);
	texture->Descriptor->AddUniformBuffers(UniformBuffers, 0, sizeof(UBO));
	texture->Descriptor->AddImage(&texture->texture, GlobalImageSampler, 1);
	texture->Descriptor->Create();
}


void VulkanRenderer::SubmitSpriteBatch(const SpriteBatch& spriteBatch) {
	currentSpriteBatch = spriteBatch;
}


void VulkanRenderer::createInstance(const std::string& appName, const Version& gameVersion) {
		//Información de la app.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION((int)gameVersion.Mayor, (int)gameVersion.Menor, (int)gameVersion.Parche);
		appInfo.pEngineName = "OSKengine";
		appInfo.engineVersion = VK_MAKE_VERSION((int)ENGINE_VERSION_MAYOR_NUMERIC, (int)ENGINE_VERSION_MINOR, 0);
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
			"VK_LAYER_KHRONOS_validation"
		};

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugConsoleCreateInfo{};
		debugConsoleCreateInfo = {};
		debugConsoleCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugConsoleCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugConsoleCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugConsoleCreateInfo.pfnUserCallback = DebugCallback;
		debugConsoleCreateInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugConsoleCreateInfo;
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


void VulkanRenderer::setupDebugConsole() {
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
#endif
}


void VulkanRenderer::createSurface() {
		//Obtener la superficie.
		VkResult result = glfwCreateWindowSurface(Instance, Window->GetGLFWWindow(), nullptr, &Surface);

		//Error-handling.
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear superficie.");
	}


void VulkanRenderer::getGPU() {
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

	if (GPU == nullptr)
		Logger::Log(LogMessageLevels::CRITICAL_ERROR, " no se encuentra ninguna GPU compatible.");
}


void VulkanRenderer::createLogicalDevice() {
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


void VulkanRenderer::createSwapchain() {
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


void VulkanRenderer::createSwapchainImageViews() {
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


void VulkanRenderer::createRenderpass() {
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
	sbPass.Set(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

	renderpass->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
	renderpass->AddSubpass(sbPass);
	renderpass->AddAttachment(clrAttachment);
	renderpass->AddAttachment(dpthAttachment);
	renderpass->Create();
}


void VulkanRenderer::createDescriptorSetLayout() {
	DescLayout = CreateNewDescriptorLayout();
	DescLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT); //ALBEDO
	DescLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	DescLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	DescLayout->AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	DescLayout->Create(Settings.MaxTextures);
}


OskResult VulkanRenderer::createGraphicsPipeline2D() {
	GraphicsPipeline2D = CreateNewGraphicsPipeline(Settings.VertexShaderPath2D, Settings.FragmentShaderPath2D);
	GraphicsPipeline2D->SetViewport({ 0, 0, (float)SwapchainExtent.width, (float)SwapchainExtent.height });
	GraphicsPipeline2D->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	GraphicsPipeline2D->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	GraphicsPipeline2D->SetDepthStencil(false);
	GraphicsPipeline2D->SetPushConstants(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConst2D));
	GraphicsPipeline2D->SetLayout(&DescLayout->VulkanDescriptorSetLayout);
	OskResult result = GraphicsPipeline2D->Create(renderpass);
	if (result != OskResult::SUCCESS)
		OSK_SHOW_TRACE();

	return result;
}


OskResult VulkanRenderer::createGraphicsPipeline3D() {
	GraphicsPipeline3D = CreateNewPhongPipeline(Settings.VertexShaderPath3D, Settings.FragmentShaderPath3D);

	return OskResult::SUCCESS;
}


void VulkanRenderer::createCommandPool() {
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


void VulkanRenderer::createDepthResources() {
	VkFormat depthFormat = getDepthFormat();
	createImage(&DepthImage, SwapchainExtent.width, SwapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	createImageView(&DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}


void VulkanRenderer::createFramebuffers() {
	Framebuffers.resize(SwapchainImageViews.size());

	//Crear los framebuffers.
	for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			SwapchainImageViews[i],
			DepthImage.View
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->VulkanRenderpass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = SwapchainExtent.width;
		framebufferInfo.height = SwapchainExtent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(LogicalDevice, &framebufferInfo, nullptr, &Framebuffers[i]);

		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}


void VulkanRenderer::createGlobalImageSampler() {
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


void VulkanRenderer::createDefaultUniformBuffers() {
	VkDeviceSize size = sizeof(UBO);
	UniformBuffers.resize(SwapchainImages.size());

	for (uint32_t i = 0; i < UniformBuffers.size(); i++)
		CreateBuffer(UniformBuffers[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	size = lights.Size();
	LightsUniformBuffers.resize(SwapchainImages.size());

	for (uint32_t i = 0; i < LightsUniformBuffers.size(); i++)
		CreateBuffer(LightsUniformBuffers[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}


void VulkanRenderer::createDescriptorPool() {

}


void VulkanRenderer::createSyncObjects() {
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


void VulkanRenderer::createCommandBuffers() {
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


void VulkanRenderer::closeSwapchain() {
	destroyImage(&DepthImage);

	for (const auto& i : Framebuffers)
		vkDestroyFramebuffer(LogicalDevice, i, nullptr);

	delete GraphicsPipeline2D;
	delete GraphicsPipeline3D;
	delete SkyboxGraphicsPipeline;

	for (const auto& i : SwapchainImageViews)
		vkDestroyImageView(LogicalDevice, i, nullptr);

	vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
}


void VulkanRenderer::Close() {
	closeSwapchain();

	delete Content;

	vkDestroySampler(LogicalDevice, GlobalImageSampler, nullptr);

	for (auto& i : UniformBuffers)
		DestroyBuffer(i);

	for (auto& i : LightsUniformBuffers)
		DestroyBuffer(i);

	vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

	delete DescLayout;
	delete PhongDescriptorLayout;
	delete SkyboxDescriptorLayout;

	delete renderpass;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
		vkDestroyFence(LogicalDevice, fences[i], nullptr);
	}

	vkDestroyDevice(LogicalDevice, nullptr);
#ifdef OSK_DEBUG
	DestroyDebugUtilsMessengerEXT(Instance, debugConsole, nullptr);
#endif
	vkDestroySurfaceKHR(Instance, Surface, nullptr);

	//Destruir la instancia.
	vkDestroyInstance(Instance, nullptr);
}


void VulkanRenderer::RecreateSwapchain() {
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

	createSwapchain();
	createSwapchainImageViews();
	createRenderpass();
	
	createGraphicsPipeline2D();
	createGraphicsPipeline3D();
	SkyboxGraphicsPipeline = CreateNewSkyboxPipeline(Settings.SkyboxVertexPath, Settings.SkyboxFragmentPath);

	if (Scene != nullptr) {
		Scene->PhongPipeline = GraphicsPipeline3D;
		Scene->SkyboxPipeline = SkyboxGraphicsPipeline;

		Scene->SetGraphicsPipeline(GraphicsPipeline3D);
	}

	createDepthResources();
	createFramebuffers();
}


void VulkanRenderer::CreateBuffer(VulkanBuffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(LogicalDevice, &bufferInfo, nullptr, &buffer.Buffer);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear buffer.");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(LogicalDevice, buffer.Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits, prop);

	result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &buffer.Memory);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: alloc mem.");

	vkBindBufferMemory(LogicalDevice, buffer.Buffer, buffer.Memory, 0);
}


void VulkanRenderer::SetRenderizableScene(RenderizableScene* scene) {
	Scene = scene;
}


void VulkanRenderer::DestroyBuffer(VulkanBuffer& buffer) const {
	vkDestroyBuffer(LogicalDevice, buffer.Buffer, nullptr);
	vkFreeMemory(LogicalDevice, buffer.Memory, nullptr);
}


void VulkanRenderer::createImageView(VulkanImage* img, VkFormat format, VkImageAspectFlags aspect, VkImageViewType type, const uint32_t& layerCount, const uint32_t& mipLevels) const {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = img->Image;
	viewInfo.viewType = type;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = layerCount;

	VkResult result = vkCreateImageView(LogicalDevice, &viewInfo, nullptr, &img->View);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear image view.");
}


void VulkanRenderer::createImage(VulkanImage* image, const uint32_t& width, const uint32_t& height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, const uint32_t& arrayLevels, VkImageCreateFlagBits flags, const uint32_t& mipLevels) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;//VK_IMAGE_VIEW_TYPE_CUBE
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = arrayLevels;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = flags;

	VkResult result = vkCreateImage(LogicalDevice, &imageInfo, nullptr, &image->Image);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear imagen.");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(LogicalDevice, image->Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits, properties);

	result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &image->Memory);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: alloc memoria de la imagen.");

	vkBindImageMemory(LogicalDevice, image->Image, image->Memory, 0);
}


void VulkanRenderer::destroyImage(VulkanImage* img) const {
	vkDestroyImage(LogicalDevice, img->Image, nullptr);
	vkDestroyImageView(LogicalDevice, img->View, nullptr);
	vkFreeMemory(LogicalDevice, img->Memory, nullptr);
}


std::vector<VkCommandBuffer> VulkanRenderer::GetCommandBuffers() {
	for (auto& i : currentSpriteBatch.spritesToDraw)
		if (i.hasChanged)
			updateSpriteVertexBuffer(&i);

	for (size_t i = 0; i < CommandBuffers.size(); i++) {
		vkResetCommandBuffer(CommandBuffers[i], 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(CommandBuffers[i], &beginInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar command buffer.");

		//Comenzar el renderpass.
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass->VulkanRenderpass;
		renderPassInfo.framebuffer = Framebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = SwapchainExtent;
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		//Comenzar el renderizado.
		vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	return CommandBuffers;
}


void VulkanRenderer::updateCommandBuffers() {
	for (auto& i : currentSpriteBatch.spritesToDraw)
		if (i.hasChanged)
			updateSpriteVertexBuffer(&i);

	for (size_t i = 0; i < CommandBuffers.size(); i++) {
		vkResetCommandBuffer(CommandBuffers[i], 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(CommandBuffers[i], &beginInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar command buffer.");

		//Comenzar el renderpass.
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass->VulkanRenderpass;
		renderPassInfo.framebuffer = Framebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = SwapchainExtent;
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		//Comenzar el renderizado.
		vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		Texture* lastImg = nullptr;

		if (Scene != nullptr) {
			Scene->Draw(CommandBuffers[i], i);
		}

		//Skybox:
		/*{
			SkyboxGraphicsPipeline->Bind(CommandBuffers[i]);

			LevelSkybox.Bind(CommandBuffers[i], SkyboxGraphicsPipeline, i);
			LevelSkybox.Draw(CommandBuffers[i]);
		}

		//3D
		{
			GraphicsPipeline3D->Bind(CommandBuffers[i]);

			model.Bind(CommandBuffers[i]);
			model.texture->PhongDescriptorSet->Bind(CommandBuffers[i], GraphicsPipeline3D, i);
			PushConst3D push;
			push.model = glm::mat4(1.0);
			vkCmdPushConstants(CommandBuffers[i], GraphicsPipeline3D->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &push);
			model.Draw(CommandBuffers[i]);			
		}*/
		//3D
		
		//2D
		if (!currentSpriteBatch.spritesToDraw.empty()) {
			GraphicsPipeline2D->Bind(CommandBuffers[i]);
			vkCmdBindIndexBuffer(CommandBuffers[i], currentSpriteBatch.spritesToDraw[0].IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
	
			for (Sprite sprite : currentSpriteBatch.spritesToDraw) {
				VkBuffer vertexBuffers[] = { sprite.VertexBuffer.Buffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
				
				if (sprite.texture != lastImg) {
					sprite.texture->Descriptor->Bind(CommandBuffers[i], GraphicsPipeline2D, i);
					lastImg = sprite.texture;
				}
				PushConst2D pConst = sprite.getPushConst();
				vkCmdPushConstants(CommandBuffers[i], GraphicsPipeline2D->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(sprite.Indices.size()), 1, 0, 0, 0);
			}
		
		}
		//2D
		
		vkCmdEndRenderPass(CommandBuffers[i]);
		result = vkEndCommandBuffer(CommandBuffers[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: grabar renderpass.");
	}
}


VulkanImage VulkanRenderer::createImageFromBitMap(uint32_t width, uint32_t height, uint8_t* pixels) {
	VulkanImage image{};
	createImage(&image, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkDeviceSize imageSize = (VkDeviceSize)width * height * 4;
	std::vector<uint8_t> nPixels{};
	for (int i = 0; i < imageSize / 4; i++) {
		nPixels.push_back(pixels[i]);
		nPixels.push_back(pixels[i]);
		nPixels.push_back(pixels[i]);
		nPixels.push_back(pixels[i]);
	}
	VulkanBuffer stagingBuffer{};
	CreateBuffer(stagingBuffer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, imageSize, 0, &data);
	memcpy(data, nPixels.data(), static_cast<size_t>(imageSize));
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	transitionImageLayout(&image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(&stagingBuffer, &image, width, height);
	transitionImageLayout(&image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DestroyBuffer(stagingBuffer);

	createImageView(&image, VK_FORMAT_R8G8B8A8_SRGB);

	return image;
}


void VulkanRenderer::createVertexBuffer(VulkanRenderizableObject* obj) const {
	VkDeviceSize size = sizeof(Vertex) * obj->Vertices.size();
	std::cout << size << std::endl;
	//Buffer temporal CPU-GPU.
	VulkanBuffer stagingBuffer;
	CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, obj->Vertices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	//Buffer final en la GPU.
	CreateBuffer(obj->VertexBuffer, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CopyBuffer(stagingBuffer, obj->VertexBuffer, size);

	DestroyBuffer(stagingBuffer);
}


void VulkanRenderer::createIndexBuffer(VulkanRenderizableObject* obj) const {
	VkDeviceSize size = sizeof(obj->Indices[0]) * obj->Indices.size();

	VulkanBuffer stagingBuffer;
	CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, obj->Indices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	CreateBuffer(obj->IndexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	CopyBuffer(stagingBuffer, obj->IndexBuffer, size);

	DestroyBuffer(stagingBuffer);
}


void VulkanRenderer::createSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	CreateBuffer(sprite->VertexBuffer, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	/*VulkanBuffer stagingBuffer;
	createBuffer(&stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Vertices.data(), size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);*/

	updateSpriteVertexBuffer(sprite);
}


void VulkanRenderer::createSpriteIndexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(sprite->Indices[0]) * sprite->Indices.size();

	VulkanBuffer stagingBuffer;
	CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Indices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	CreateBuffer(sprite->IndexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	CopyBuffer(stagingBuffer, sprite->IndexBuffer, size);

	DestroyBuffer(stagingBuffer);
}


void VulkanRenderer::updateSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	void* data;
	vkMapMemory(LogicalDevice, sprite->VertexBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Vertices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, sprite->VertexBuffer.Memory);

	sprite->hasChanged = false;
}


//Copia el contenido de un buffer a otro buffer.
void VulkanRenderer::CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = sourceOffset;
	copyRegion.dstOffset = destinationOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, source.Buffer, destination.Buffer, 1, &copyRegion);

	endSingleTimeCommandBuffer(cmdBuffer);
}


VkCommandBuffer VulkanRenderer::beginSingleTimeCommandBuffer() const {
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


void VulkanRenderer::endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const {
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(GraphicsQ, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQ);

	vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &cmdBuffer);
}


void VulkanRenderer::copyBufferToImage(VulkanBuffer* buffer, VulkanImage* img, const uint32_t& width, const uint32_t& height) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(cmdBuffer, buffer->Buffer, img->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommandBuffer(cmdBuffer);
}


void VulkanRenderer::transitionImageLayout(VulkanImage* img, VkImageLayout oldLayout, VkImageLayout newLayout, const uint32_t& mipLevels, const uint32_t& arrayLevels) const {
	VkCommandBuffer cmdBuffer = beginSingleTimeCommandBuffer();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = img->Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = arrayLevels;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommandBuffer(cmdBuffer);
}


//Obtiene el tipo de memoria indicado.
uint32_t VulkanRenderer::getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const {
	//Tipos de memoria disponibles.
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(GPU, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if (memoryTypeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;

	return -1;

	throw std::runtime_error("failed to find suitable memory type!");
}


VkFormat VulkanRenderer::getDepthFormat() const {
	return getSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void VulkanRenderer::createDescriptorSets(Texture* texture) const {
	texture->Descriptor = CreateNewDescriptorSet();
	texture->Descriptor->SetDescriptorLayout(DescLayout);
	texture->Descriptor->AddUniformBuffers(UniformBuffers, 0, sizeof(UBO));
	texture->Descriptor->AddImage(&texture->Albedo, GlobalImageSampler, 1);
	texture->Descriptor->AddUniformBuffers(LightsUniformBuffers, 2, sizeof(LightUBO));
	texture->Descriptor->Create();
}


void VulkanRenderer::destroyTexture(Texture* texture) const {
	destroyImage(&texture->Albedo);
	texture = nullptr;
}


GPUinfo VulkanRenderer::getGPUinfo(VkPhysicalDevice gpu) const {
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
		
	return info;
}


VkPresentModeKHR VulkanRenderer::getPresentMode(const std::vector<VkPresentModeKHR>& modes) const {
	VkPresentModeKHR finalPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkPresentModeKHR targetPresentMode = translatePresentMode(this->targetPresentMode);

	for (const auto& mode : modes)
		if (mode == targetPresentMode)
			finalPresentMode = mode;

	return finalPresentMode;
}


VkExtent2D VulkanRenderer::getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
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


VkFormat VulkanRenderer::getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
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


SwapchainSupportDetails VulkanRenderer::getSwapchainSupportDetails(VkPhysicalDevice gpu) const {
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


QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice gpu) const {
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


VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}


VkBool32 VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		//Message severity:
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

		//Tipos de mensaje:
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

		Logger::Log(LogMessageLevels::BAD_ERROR, pCallbackData->pMessage);

		return VK_FALSE;
	}
