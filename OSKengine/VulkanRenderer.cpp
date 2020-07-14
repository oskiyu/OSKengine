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

#include <ft2build.h>
#include <thread>
#include FT_FREETYPE_H

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


void VulkanRenderer::Init(const RenderMode& mode, const std::string& appName, const Version& gameVersion) {
	renderMode = mode;

	createInstance(appName, gameVersion);
	setupDebugConsole();
	createSurface();
	getGPU();
	createLogicalDevice();
	createSwapchain();
	createSwapchainImageViews();
	createRenderpass();
	createDescriptorSetLayout();

	if (renderMode == RenderMode::RENDER_MODE_2D || renderMode == RenderMode::RENDER_MODE_2D_AND_3D)
		createGraphicsPipeline2D();

	if (renderMode == RenderMode::RENDER_MODE_3D || renderMode == RenderMode::RENDER_MODE_2D_AND_3D)
		createGraphicsPipeline3D();

	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createGlobalImageSampler();
	createDefaultUniformBuffers();
	createDescriptorPool();
	createSyncObjects();
	createCommandBuffers();

	createVertexBuffer(&sprite);
	createIndexBuffer(&sprite);

	textures.reserve(Settings.MaxTextures);
	textureFromString.reserve(Settings.MaxTextures);

	DefaultCamera2D = Camera2D(Window);

	hasBeenInit = true;
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

	vkWaitForFences(LogicalDevice, 1, &InFlightFences[renderVars.currentImage], VK_TRUE, UINT64_MAX);
	
	DefaultCamera2D.Update();
	for (size_t i = 0; i < UniformBuffers2D.size(); i++) {
		void* data;
		vkMapMemory(LogicalDevice, UniformBuffers2D[i].Memory, 0, sizeof(UniformBuffer2D), 0, &data);
		UniformBuffer2D ubo { DefaultCamera2D.getUBO() };
		memcpy(data, &ubo, sizeof(UniformBuffer2D));
		vkUnmapMemory(LogicalDevice, UniformBuffers2D[i].Memory);
	}

	if (Settings.AutoUpdateCommandBuffers)
		updateCommandBuffers();
	//Repreenta cual es la imagen que se va a renderizar.
	uint32_t imageID = 0;

	//Adquiere la siguiente imágen sobre la que se va a renderizar.
	VkResult result = vkAcquireNextImageKHR(LogicalDevice, Swapchain, UINT64_MAX, ImageAvailableSemaphores[renderVars.currentImage], VK_NULL_HANDLE, &imageID);

	//La ventana ha cambiado de tamaño.
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapchain();

		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("ERROR: obtener imagen.");

	if (ImagesInFlight[imageID] != nullptr)
		vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageID], VK_TRUE, UINT64_MAX);
	ImagesInFlight[imageID] = InFlightFences[renderVars.currentImage];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Esperar a que la imagen esté disponible antes de renderizar.
	VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[renderVars.currentImage] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[imageID];

	//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
	VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[renderVars.currentImage] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(LogicalDevice, 1, &InFlightFences[renderVars.currentImage]);

	result = vkQueueSubmit(GraphicsQ, 1, &submitInfo, InFlightFences[renderVars.currentImage]);
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
	presentInfo.pImageIndices = &imageID;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(PresentQ, &presentInfo);

	//La ventana ha cambiado de tamaño.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderVars.hasFramebufferBeenResized) {
		RecreateSwapchain();
		renderVars.hasFramebufferBeenResized = false;
	}

	vkQueueWaitIdle(PresentQ);

	renderVars.currentImage = (renderVars.currentImage + 1) % MAX_FRAMES_IN_FLIGHT;

	double endTime = glfwGetTime();

	double targetMS = 1 / FPSlimit;
	if ((endTime - startTime) < targetMS)
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(targetMS * 1000 - (endTime - startTime) * 1000)));
}


SpriteBatch VulkanRenderer::CreateSpriteBatch() {
	SpriteBatch spriteBatch{};
	spriteBatch.renderer = this;

	return spriteBatch;
}


Texture* VulkanRenderer::LoadTexture(const std::string& path) {
	if (textureFromString.find(path) != textureFromString.end())
		return textureFromString[path];

	Texture loadedTexture{};
			
	int width;
	int height;
	int nChannels;

	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
	VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;

	VulkanBuffer stagingBuffer;
	createBuffer(&stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(size));
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	stbi_image_free(pixels);
	createImage(&loadedTexture.image, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	transitionImageLayout(&loadedTexture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(&stagingBuffer, &loadedTexture.image, width, height);
	transitionImageLayout(&loadedTexture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	destroyBuffer(&stagingBuffer);

	createImageView(&loadedTexture.image, VK_FORMAT_R8G8B8A8_SRGB);

	createDescriptorSets(&loadedTexture);

	textures.push_back(loadedTexture);
	textureFromString[path] = &textures.back();
	
	return textureFromString[path];
}


void VulkanRenderer::LoadFont(Font& fuente, const std::string& source, uint32_t size) {
	static FT_Library ftLib = nullptr;
	if (ftLib == nullptr)
		if (FT_Init_FreeType(&ftLib))
			throw std::runtime_error("ERROR: cargar fuente.");

	FT_Face face;

	FT_Error result = FT_New_Face(ftLib, source.c_str(), 0, &face);;
	if (result)
		Logger::Log(LogMessageLevels::BAD_ERROR, "cargar fuente " + source, __LINE__);

	FT_Set_Pixel_Sizes(face, 0, size);

	FT_Face lastFace = nullptr;
	for (uint8_t c = 0; c < 255; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "freetype: load glyph: " + std::to_string(c) + " de " + source, __LINE__);
			continue;
		}

		if (face->glyph->bitmap.width <= 0 || face->glyph->bitmap.rows <= 0) {
			FontChar character{};
			character.sprite.texture = nullptr;
			character.Size = Vector2(lastFace->glyph->bitmap.width, lastFace->glyph->bitmap.rows);
			character.Bearing = Vector2(lastFace->glyph->bitmap_left, lastFace->glyph->bitmap_top);
			character.Advance = lastFace->glyph->advance.x;

			fuente.Characters[c] = character;

			continue;
		}

		Texture texture{};
		VulkanImage image = createImageFromBitMap(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
		texture.image = image;
		createDescriptorSets(&texture);

		textures.push_back(texture);

		FontChar character{};
		character.sprite.texture = &textures.back();
		character.sprite.Vertices[0].TextureCoordinates.x = -1.0f;
		character.sprite.Vertices[3].TextureCoordinates.x = -1.0f;
		createSpriteVertexBuffer(&character.sprite);
		createSpriteIndexBuffer(&character.sprite);
		character.Size = Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		character.Bearing = Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		character.Advance = face->glyph->advance.x;
		
		fuente.Characters[c] = character;

		lastFace = face;
	}

	FT_Done_Face(face);

	fuente.Size = size;
}


void VulkanRenderer::LoadSprite(Sprite* sprite, const std::string& path) {
	sprite->texture = LoadTexture(path);

	createSpriteVertexBuffer(sprite);
	createSpriteIndexBuffer(sprite);
}


void VulkanRenderer::SubmitSpriteBatch(const SpriteBatch& spriteBatch) {
	currentSpriteBatch = spriteBatch;
}


void VulkanRenderer::UpdateDefaultUBO(void* ubo) {
	/*for (size_t i = 0; i < defaultUniformBuffers.size(); i++) {
		void* data;
		vkMapMemory(LogicalDevice, defaultUniformBuffers[i].Memory, 0, Settings.DefaultUBOSize, 0, &data);
		memcpy(data, &ubo, sizeof(Settings.DefaultUBOSize));
		vkUnmapMemory(LogicalDevice, defaultUniformBuffers[i].Memory);
	}*/
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
			Logger::Log(LogMessageLevels::CRITICAL_ERROR, "Crear instancia de Vulkan.");
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
	//Un solo buffer (una imagen).
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = SwapchainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//LoadOp: qué se hace al renderizarsobre  una imagen.
	//	VK_ATTACHMENT_LOAD_OP_LOAD: se conserva lo que había antes.
	//	VK_ATTACHMENT_LOAD_OP_CLEAR: se limpia.
	//	VK_ATTACHMENT_LOAD_OP_DONT_CARE: da igual.
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//Stencil.
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = getDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Subpasses.
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	const std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(LogicalDevice, &renderPassInfo, nullptr, &Renderpass);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear renderpass.");
}


void VulkanRenderer::createDescriptorSetLayout() {
	//UBO.
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//Texture.
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(LogicalDevice, &layoutInfo, nullptr, &DescriptorSetLayout);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear descriptor set layout.");
}


void VulkanRenderer::createGraphicsPipeline2D() {
	//Código de los shaders.
	const std::vector<char> vertexCode = FileIO::ReadBinaryFromFile(Settings.VertexShaderPath2D);
	const std::vector<char> fragmentCode = FileIO::ReadBinaryFromFile(Settings.FragmentShaderPath2D);

	//Shader modules.
	VkShaderModule vertexModule = getShaderModule(vertexCode);
	VkShaderModule fragmentModule = getShaderModule(fragmentCode);

	//Crear los shaders.
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = fragmentModule;
	fragmentShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	//Información que le pasamos a los shaders.
	//Vértices.
	const auto bindingDesc = Vertex::GetBindingDescription();
	const auto attribDesc = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDesc.size());
	vertexInputInfo.pVertexAttributeDescriptions = attribDesc.data();

	//Primitivos a dibujar.
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//Viewport: tamaño de la imagen que se va a renderizar.
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)SwapchainExtent.width;
	viewport.height = (float)SwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Tijeras: renderizamos todo.
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; //FALSE: si el objeto está fuera de los límites no se renderiza.
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	//Modo de polígonos:
	//	VK_POLYGON_MODE_FILL
	//	VK_POLYGON_MODE_LINE
	//	VK_POLYGON_MODE_POINT: vértice -> punto.
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	//MSAA (disabled).
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//Qué hacer al sobreesxribir un píxel.
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_FALSE;
	depthStencilCreateInfo.depthWriteEnable = VK_FALSE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.minDepthBounds = 0.0f;
	depthStencilCreateInfo.maxDepthBounds = 1.0f;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilCreateInfo.front = {};
	depthStencilCreateInfo.back = {};

	//PipelineLayout
	VkPushConstantRange pushConstRange{};
	pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstRange.offset = 0;
	pushConstRange.size = sizeof(PushConst2D);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstRange;

	VkResult result = vkCreatePipelineLayout(LogicalDevice, &pipelineLayoutInfo, nullptr, &PipelineLayout2D);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear pipelielayout.");

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencilCreateInfo;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = PipelineLayout2D;
	pipelineInfo.renderPass = Renderpass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(LogicalDevice, nullptr, 1, &pipelineInfo, nullptr, &GraphicsPipeline2D);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear pipeline.");

	//Cleanup.
	vkDestroyShaderModule(LogicalDevice, vertexModule, nullptr);
	vkDestroyShaderModule(LogicalDevice, fragmentModule, nullptr);
}


void VulkanRenderer::createGraphicsPipeline3D() {
	//Código de los shaders.
	const std::vector<char> vertexCode = FileIO::ReadBinaryFromFile(Settings.VertexShaderPath3D);
	const std::vector<char> fragmentCode = FileIO::ReadBinaryFromFile(Settings.FragmentShaderPath3D);

	//Shader modules.
	VkShaderModule vertexModule = getShaderModule(vertexCode);
	VkShaderModule fragmentModule = getShaderModule(fragmentCode);

	//Crear los shaders.
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = fragmentModule;
	fragmentShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	//Información que le pasamos a los shaders.
	//Vértices.
	const auto bindingDesc = Vertex::GetBindingDescription();
	const auto attribDesc = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDesc.size());
	vertexInputInfo.pVertexAttributeDescriptions = attribDesc.data();

	//Primitivos a dibujar.
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//Viewport: tamaño de la imagen que se va a renderizar.
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)SwapchainExtent.width;
	viewport.height = (float)SwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Tijeras: renderizamos todo.
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; //FALSE: si el objeto está fuera de los límites no se renderiza.
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	//Modo de polígonos:
	//	VK_POLYGON_MODE_FILL
	//	VK_POLYGON_MODE_LINE
	//	VK_POLYGON_MODE_POINT: vértice -> punto.
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	//MSAA (disabled).
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//Qué hacer al sobreesxribir un píxel.
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.minDepthBounds = 0.0f;
	depthStencilCreateInfo.maxDepthBounds = 1.0f;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilCreateInfo.front = {};
	depthStencilCreateInfo.back = {};

	//PipelineLayout
	VkPushConstantRange pushConstRange{};
	pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstRange.offset = 0;
	pushConstRange.size = sizeof(PushConst2D); ////////////////

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstRange;

	VkResult result = vkCreatePipelineLayout(LogicalDevice, &pipelineLayoutInfo, nullptr, &PipelineLayout3D);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear pipelielayout.");

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencilCreateInfo;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = PipelineLayout3D;
	pipelineInfo.renderPass = Renderpass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(LogicalDevice, nullptr, 1, &pipelineInfo, nullptr, &GraphicsPipeline3D);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear pipeline.");

	//Cleanup.
	vkDestroyShaderModule(LogicalDevice, vertexModule, nullptr);
	vkDestroyShaderModule(LogicalDevice, fragmentModule, nullptr);
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
		framebufferInfo.renderPass = Renderpass;
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
	VkDeviceSize size = sizeof(UniformBuffer2D);
	UniformBuffers2D.resize(SwapchainImages.size());

	for (uint32_t i = 0; i < UniformBuffers2D.size(); i++)
		createBuffer(&UniformBuffers2D[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}


void VulkanRenderer::createDescriptorPool() {
	//Tamaño del pool (número de descriptors).
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	//UBO.
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(SwapchainImages.size());
	//Textura.
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(SwapchainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(SwapchainImages.size() * Settings.MaxTextures);

	VkResult result = vkCreateDescriptorPool(LogicalDevice, &poolInfo, nullptr, &DescriptorPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear descriptor pool.");
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

	if (GraphicsPipeline3D != nullptr)
		vkDestroyPipeline(LogicalDevice, GraphicsPipeline3D, nullptr);

	if (GraphicsPipeline2D != nullptr)
		vkDestroyPipeline(LogicalDevice, GraphicsPipeline2D, nullptr);

	if (PipelineLayout2D != nullptr)
		vkDestroyPipelineLayout(LogicalDevice, PipelineLayout2D, nullptr);

	if (PipelineLayout3D != nullptr)
		vkDestroyPipelineLayout(LogicalDevice, PipelineLayout3D, nullptr);

	vkDestroyRenderPass(LogicalDevice, Renderpass, nullptr);

	for (const auto& i : SwapchainImageViews)
		vkDestroyImageView(LogicalDevice, i, nullptr);

	vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
}


void VulkanRenderer::Close() {
	closeSwapchain();

	destroyAllTextures();

	vkDestroySampler(LogicalDevice, GlobalImageSampler, nullptr);

	for (auto& i : UniformBuffers2D)
		destroyBuffer(&i);

	vkDestroyDescriptorSetLayout(LogicalDevice, DescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(LogicalDevice, DescriptorPool, nullptr);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
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
	if (GraphicsPipeline2D != nullptr)
		createGraphicsPipeline2D();
	if (GraphicsPipeline3D != nullptr)
		createGraphicsPipeline3D();
	createDepthResources();
	createFramebuffers();

	Logger::DebugLog("Recreated swapchain.");
}


void VulkanRenderer::createBuffer(VulkanBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(LogicalDevice, &bufferInfo, nullptr, &buffer->Buffer);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear buffer.");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(LogicalDevice, buffer->Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits, prop);

	result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &buffer->Memory);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: alloc mem.");

	vkBindBufferMemory(LogicalDevice, buffer->Buffer, buffer->Memory, 0);
}


void VulkanRenderer::destroyBuffer(VulkanBuffer* buffer) const {
	vkDestroyBuffer(LogicalDevice, buffer->Buffer, nullptr);
	vkFreeMemory(LogicalDevice, buffer->Memory, nullptr);
}


void VulkanRenderer::createImageView(VulkanImage* img, VkFormat format, VkImageAspectFlags aspect) const {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = img->Image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(LogicalDevice, &viewInfo, nullptr, &img->View);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear image view.");
}


void VulkanRenderer::createImage(VulkanImage* image, const uint32_t& width, const uint32_t& height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

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


void VulkanRenderer::updateCommandBuffers() {
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
		renderPassInfo.renderPass = Renderpass;
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
		//vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline3D);

		Texture* lastImg = nullptr;
		if (!currentSpriteBatch.spritesToDraw.empty()) {
			vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline2D);
			vkCmdBindIndexBuffer(CommandBuffers[i], currentSpriteBatch.spritesToDraw[0].IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
	
			for (Sprite sprite : currentSpriteBatch.spritesToDraw) {
				VkBuffer vertexBuffers[] = { sprite.VertexBuffer.Buffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
				//updateSpriteVertexBuffer(&sprite);
				//if (sprite->texture != lastImg) {
					vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout2D, 0, 1, &sprite.texture->DescriptorSets[i], 0, nullptr);
					lastImg = sprite.texture;
				//}
				PushConst2D pConst = sprite.getPushConst();
				vkCmdPushConstants(CommandBuffers[i], PipelineLayout2D, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(sprite.Indices.size()), 1, 0, 0, 0);
			}
		
		}

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
	createBuffer(&stagingBuffer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, imageSize, 0, &data);
	memcpy(data, nPixels.data(), static_cast<size_t>(imageSize));
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	transitionImageLayout(&image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(&stagingBuffer, &image, width, height);
	transitionImageLayout(&image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	destroyBuffer(&stagingBuffer);

	createImageView(&image, VK_FORMAT_R8G8B8A8_SRGB);

	return image;
}


void VulkanRenderer::createVertexBuffer(VulkanRenderizableObject* obj) const {
	VkDeviceSize size = sizeof(Vertex) * obj->Vertices.size();
	std::cout << size << std::endl;
	//Buffer temporal CPU-GPU.
	VulkanBuffer stagingBuffer;
	createBuffer(&stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, obj->Vertices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	//Buffer final en la GPU.
	createBuffer(&obj->VertexBuffer, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	copyBuffer(stagingBuffer, obj->VertexBuffer, size);

	destroyBuffer(&stagingBuffer);
}


void VulkanRenderer::createIndexBuffer(VulkanRenderizableObject* obj) const {
	VkDeviceSize size = sizeof(obj->Indices[0]) * obj->Indices.size();

	VulkanBuffer stagingBuffer;
	createBuffer(&stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, obj->Indices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	createBuffer(&obj->IndexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copyBuffer(stagingBuffer, obj->IndexBuffer, size);

	destroyBuffer(&stagingBuffer);
}


void VulkanRenderer::createSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	createBuffer(&sprite->VertexBuffer, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	updateSpriteVertexBuffer(sprite);
}


void VulkanRenderer::createSpriteIndexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(sprite->Indices[0]) * sprite->Indices.size();

	VulkanBuffer stagingBuffer;
	createBuffer(&stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Indices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

	createBuffer(&sprite->IndexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copyBuffer(stagingBuffer, sprite->IndexBuffer, size);

	destroyBuffer(&stagingBuffer);
}


void VulkanRenderer::updateSpriteVertexBuffer(Sprite* sprite) const {
	VkDeviceSize size = sizeof(Vertex) * sprite->Vertices.size();

	void* data;
	vkMapMemory(LogicalDevice, sprite->VertexBuffer.Memory, 0, size, 0, &data);
	memcpy(data, sprite->Vertices.data(), (size_t)size);
	vkUnmapMemory(LogicalDevice, sprite->VertexBuffer.Memory);
}


//Copia el contenido de un buffer a otro buffer.
void VulkanRenderer::copyBuffer(VulkanBuffer source, VulkanBuffer destination, VkDeviceSize size, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset) const {
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


void VulkanRenderer::transitionImageLayout(VulkanImage* img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const {
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
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
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

	throw std::runtime_error("failed to find suitable memory type!");
}


VkFormat VulkanRenderer::getDepthFormat() const {
	return getSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void VulkanRenderer::createDescriptorSets(Texture* texture) const {
	texture->DescriptorSets.resize(SwapchainImages.size());

	std::vector<VkDescriptorSetLayout> layouts(SwapchainImages.size(), DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapchainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(LogicalDevice, &allocInfo, texture->DescriptorSets.data());
	//if (result != VK_SUCCESS)
		//throw std::runtime_error("ERROR: crear descriptor sets.");

	for (size_t i = 0; i < SwapchainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = UniformBuffers2D[i].Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBuffer2D);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->image.View;
		imageInfo.sampler = GlobalImageSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = texture->DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0; //Binding
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = texture->DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1; //Binding
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(LogicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
}


void VulkanRenderer::destroyTexture(Texture* texture) const {
	destroyImage(&texture->image);
	texture = nullptr;
}


void VulkanRenderer::destroyAllTextures() {
	for (auto& i : textures)
		destroyTexture(&i);
}


//Crea un módulo de shader.
VkShaderModule VulkanRenderer::getShaderModule(const std::vector<char>& code) const {
	VkShaderModule output;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//Crearlo y error-handling.
	VkResult result = vkCreateShaderModule(LogicalDevice, &createInfo, nullptr, &output);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear shader module.");

	return output;
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
