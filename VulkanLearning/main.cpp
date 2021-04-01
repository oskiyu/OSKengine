#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <vector>
#include <thread>

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>

#include "QueueFamilyIndices.hpp"
#include "SwapchainSupportDetails.hpp"
#include "Vertex.hpp"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "UBO.h"
#include "VulkanObject.hpp"

#include <set>
#include <algorithm>
#include <fstream>
#include <string>
#include "PushConstants.h"
#include "VulkanToString.hpp"
#include "RenderMode.h"
#include "Scene.h"

//Tamaño inicial de la ventana.
constexpr int32_t INIT_WINDOW_WIDTH = 800;
constexpr int32_t INIT_WINDOW_HEIGHT = 600;

//Máximo de frames renderizados.
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

constexpr uint32_t MAX_TEXTURES = 32;

#define USE_RTX

#ifndef USE_RTX

//Clase que usa Vulkan.
class VulkanGame {

public:

	void Run(const RenderMode& renderMode) {
		InitWindow();
		InitVulkan(renderMode);
		MainLoop();
		Close();
	}

private:

	//Creamos la ventana.
	void InitWindow() {
		//Inicializar GLFW.
		glfwInit();

		//Para que no use OpenGL.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Creamos la ventana.
		ventana = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);

		//Callbacks.
		glfwSetWindowUserPointer(ventana, this);
		glfwSetFramebufferSizeCallback(ventana, WindowResizeCallbac);
	}


	//Inicializa Vulkan.
	void InitVulkan(const RenderMode& renderMode) {
		CreateInstance();
		SetupDebugConsole();
		CreateSurface();
		GetPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();

		if (renderMode == RenderMode::RENDER_MODE_2D || renderMode == RenderMode::RENDER_MODE_2D_AND_3D)
			CreateGraphicsPipeline2D();

		if (renderMode == RenderMode::RENDER_MODE_3D || renderMode == RenderMode::RENDER_MODE_2D_AND_3D)
			CreateGraphicsPipeline3D();

		CreateCommandPool();
		CreateDepthResources();
		CreateFramebuffers();
		CreateGlobalImageSampler();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateSyncObjects();

		CreateTextureImage(&texture, "Img/td.png");

		VulkanObject obj{};
		obj.Texture = &texture;
		objects.push_back(obj);

		obj.Vertices = {
			{{-1.5f, -0.25f, 0}, {1.0f, 0.0f, 1.0f}, {2.0f, 0.0f}},
			{{1.5f, -0.25f, 0}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.5f, 0.25f, 0}, {1.0f, 0.0f, 1.0f}, {0.0f, 2.0f}},
			{{-1.5f, 0.25f, 0}, {1.0f, 1.0f, 1.0f}, {2.0f, 2.0f}}
		};
		//objects.push_back(obj);

		for (auto& i : objects) {
			CreateVertexBuffer(&i);
			CreateIndexBuffer(&i);
			if (i.Texture == nullptr)
				i.Texture = &texture;
			CreateDescriptorSets(&i);
		}

		CreateCommandBuffers();
	}

	int ispressed = false;
	void MainLoop() {
		double lastTime = glfwGetTime();
		double currTime = lastTime;
		while (!glfwWindowShouldClose(ventana)) {
			glfwPollEvents();
			currTime = glfwGetTime();

			for (int i = 0; i < objects.size(); i++)
				UpdateObjectConstants(&objects[i], currTime - lastTime, i);

			lastTime = glfwGetTime();
			Draw();

			if (!ispressed && glfwGetKey(ventana, GLFW_KEY_ENTER) == GLFW_PRESS)
				ShowProfilingResults();
			ispressed = glfwGetKey(ventana, GLFW_KEY_ENTER);
		}

		vkDeviceWaitIdle(LogicalDevice);
	}


	void Close() {
		CloseSwapchain();

		DestroyImage(texture);
		vkDestroySampler(LogicalDevice, GlobalImageSampler, nullptr);

		for (auto& i : UniformBuffers)
			DestroyBuffer(i);

		vkDestroyDescriptorSetLayout(LogicalDevice, DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(LogicalDevice, DescriptorPool, nullptr);

		for (auto& i : objects) {
			DestroyBuffer(i.VertexBuffer);
			DestroyBuffer(i.IndexBuffer);
		}

		DestroySyncObjects();

		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

		DestroyLogicalDevice();
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(Instance, DebugConsole, nullptr);
#endif
		DestroySurface();

		//Destruir la instancia.
		vkDestroyInstance(Instance, nullptr);

		//Destruir la ventana.
		glfwDestroyWindow(ventana);
		glfwTerminate();
	}


	//Crea la instancia de Vulkan.
	void CreateInstance() {
		//Inicializar capas de validación.
#ifdef _DEBUG
		if (!CheckValidationLayerSupport())
			throw new std::runtime_error("ERROR: no se pueden iniciar las capas de validación.");
#endif

		//Información de la app.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "OSKengine Vk";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		//Create info.
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//Establecemos las extensiones.
		auto extensions = GetExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

#ifdef _DEBUG
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugConsoleCreateInfo{};
		GetDebugMessengerCreateInfo(debugConsoleCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugConsoleCreateInfo;
#endif

		//Crear la instancia y error-handling.
		VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);

		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear instancia de Vulkan.");

		//Output.
#ifdef _DEBUG
		std::cout << "Extensiones: " << std::endl;
		for (const auto& e : extensions)
			std::cout << e << std::endl;
#endif

	}


	//Comprueba si se pueden usar capas de validación.
	bool CheckValidationLayerSupport() {
		//Número de capas.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		//Obtener las capas disponibles.
		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		//Comprobación
		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : layers) {
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


	//Extensiones a usar.
	std::vector<const char*> GetExtensions() const {
		//Extensiones de la ventana.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		//Extensiones totales.
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}


	//Obtiene la GPU.
	void GetPhysicalDevice() {
		//Obtiene el número de GPUs disponibles.
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(Instance, &count, nullptr);

		if (count == 0)
			throw std::runtime_error("ERROR: no se encuentra ninguna GPU compatible.");

		//Obtiene las GPUs.
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(Instance, &count, devices.data());

		//Comprobar la compatibilidad de las GPUs.
		//Obtener una GPU compatible.
		for (const auto& gpu : devices) {
			if (IsGPUsuitable(gpu)) {
				GPU = gpu;

				break;
			}
		}

		if (GPU == nullptr)
			throw std::runtime_error("ERROR: no se encuentra ninguna GPU compatible.");
	}


	//Crea el logical device.
	void CreateLogicalDevice() {
		//Establecer las colas que vamos a usar.
		QueueFamilyIndices indices = FindQueueFamilies(GPU);

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


	//Destruye el logical device.
	void DestroyLogicalDevice() {
		vkDestroyDevice(LogicalDevice, nullptr);
	}


	//Crea la superficie.
	void CreateSurface() {
		//Obtener la superficie.
		VkResult result = glfwCreateWindowSurface(Instance, ventana, nullptr, &Surface);

		//Error-handling.
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear superficie.");

	}


	//Destruye la superficie.
	void DestroySurface() {
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
	}


	//Crea el swapchain.
	void CreateSwapchain() {
		//Soporte del swapchain.
		SwapchainSupportDetails swapchainDetails = GetSwapchainSupportDetails(GPU);

		//Formato del swapchain.
		VkSurfaceFormatKHR surfaceFormat = GetSwapchainFormat(swapchainDetails.Formats);

		//Modo de pressentación.
		VkPresentModeKHR presentMode = GetPresentMode(swapchainDetails.PresentModes);

		//tamaño.
		VkExtent2D extent = GetSwapchainExtent(swapchainDetails.SurfaceCapabilities);

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
		const QueueFamilyIndices indices = FindQueueFamilies(GPU);
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


	//Crea los image views del swapchain.
	void CreateSwapchainImageViews() {
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


	//Crea el graphics pipeline.
	void CreateGraphicsPipeline3D() {
		//Código de los shaders.
		const std::vector<char> vertexCode = ReadFile("Shaders/vert.spv");
		const std::vector<char> fragmentCode = ReadFile("Shaders/frag.spv");

		//Shader modules.
		VkShaderModule vertexModule = GetShaderModule(vertexCode);
		VkShaderModule fragmentModule = GetShaderModule(fragmentCode);

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
		const auto attribDesc = Vertex::GetAttributeDescriptions_FullVertex();

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
		pushConstRange.size = sizeof(PushConst);

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
		pipelineInfo.renderPass = RenderPass;
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


	//Crea el graphics pipeline.
	void CreateGraphicsPipeline2D() {
		//Código de los shaders.
		const std::vector<char> vertexCode = ReadFile("Shaders/2D/vert.spv");
		const std::vector<char> fragmentCode = ReadFile("Shaders/2D/frag.spv");

		//Shader modules.
		VkShaderModule vertexModule = GetShaderModule(vertexCode);
		VkShaderModule fragmentModule = GetShaderModule(fragmentCode);

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
		const auto attribDesc = Vertex::GetAttributeDescriptions_FullVertex();

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
		pushConstRange.size = sizeof(PushConst);

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
		pipelineInfo.renderPass = RenderPass;
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


	//Crea un renderpass.
	void CreateRenderPass() {
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
		depthAttachment.format = GetDepthFormat();
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

		VkResult result = vkCreateRenderPass(LogicalDevice, &renderPassInfo, nullptr, &RenderPass);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear renderpass.");
	}


	//Crea los framebuffers.
	void CreateFramebuffers() {
		Framebuffers.resize(SwapchainImageViews.size());

		//Crear los framebuffers.
		for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments = {
				SwapchainImageViews[i],
				DepthImage.View
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = RenderPass;
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


	//Crea el command pool.
	void CreateCommandPool() {
		QueueFamilyIndices indices = FindQueueFamilies(GPU);

		//Para la graphics q.
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult result = vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &CommandPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear command pool.");
	}


	//Crea los command buffers.
	void CreateCommandBuffers() {
		if (CommandBuffers.size() != Framebuffers.size())
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


	void CreateTextureImage(VulkanImage* img, const std::string& path) {
		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
		VkDeviceSize size = width * height * 4;

		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);
		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(size));
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		stbi_image_free(pixels);
		CreateImage(width, height, img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransitionImageLayout(img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, img, width, height);
		TransitionImageLayout(img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		DestroyBuffer(stagingBuffer);

		CreateImageView(img, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void CreateImageView(VulkanImage* img, VkFormat format, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT) const {
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

	void CreateGlobalImageSampler() {
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

	VulkanImage texture;

	void CreateImage(int width, int height, VulkanImage* image, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
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
		allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &image->Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc memoria de la imagen.");

		vkBindImageMemory(LogicalDevice, image->Image, image->Memory, 0);
	}

	void UpdateCommandBuffers() {
		//Grabar los comandos.
		double oldTime = glfwGetTime();
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
			renderPassInfo.renderPass = RenderPass;
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
			vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline3D);

			VulkanImage* lastImg = nullptr;
			for (const auto& obj : objects) {
				double time = glfwGetTime();

				VkBuffer vertexBuffers[] = { obj.VertexBuffer.Buffer };
				VkDeviceSize offsets[] = { 0 };
				if (obj.Texture != lastImg) {
					vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout3D, 0, 1, &obj.DescriptorSets[i], 0, nullptr);
					lastImg = obj.Texture;
				}
				vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(CommandBuffers[i], obj.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdPushConstants(CommandBuffers[i], PipelineLayout3D, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst), &obj.PushConstant);
				vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(obj.Indices.size()), 1, 0, 0, 0);

				Profiling.LastPerObjectCommandBufferTime = glfwGetTime() - time;
			}

			vkCmdEndRenderPass(CommandBuffers[i]);
			result = vkEndCommandBuffer(CommandBuffers[i]);
			if (result != VK_SUCCESS)
				throw std::runtime_error("ERROR: grabar renderpass.");
		}

		Profiling.LastUpdateCommandBuffersTime = glfwGetTime() - oldTime;
	}

	void ShowProfilingResults() {
		std::cout << "UpdateCommandBuffers(" << CommandBuffers.size() << "): " << Profiling.LastUpdateCommandBuffersTime << std::endl;
		std::cout << "Single object draw call: " << Profiling.LastPerObjectCommandBufferTime << std::endl;
		std::cout << "Copy UBO: " << Profiling.LastCopyUBOTime << std::endl;
	}

	void DestroyImage(VulkanImage& img) const {
		vkDestroyImageView(LogicalDevice, img.View, nullptr);
		vkDestroyImage(LogicalDevice, img.Image, nullptr);
		vkFreeMemory(LogicalDevice, img.Memory, nullptr);
	}

	VkCommandBuffer BeginSingleTimeCommandBuffer() const {
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

	//Cambiar el layout de la imagen, si fuera necesario.
	void TransitionImageLayout(VulkanImage* img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

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

		EndSingleTimeCommandBuffer(cmdBuffer);
	}

	void CopyBufferToImage(const VulkanBuffer& buffer, VulkanImage* img, const uint32_t& width, const uint32_t& height) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

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

		vkCmdCopyBufferToImage(cmdBuffer, buffer.Buffer, img->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommandBuffer(cmdBuffer);
	}

	void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) const {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQ, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQ);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &commandBuffer);
	}

	//Establece las variables de sincronización CPU-GPU.
	void CreateSyncObjects() {
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

	void DestroySyncObjects() {
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);

			InFlightFences[i] = nullptr;
		}
	}

	//
	void CloseSwapchain() {
		DestroyImage(DepthImage);

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

		vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

		for (const auto& i : SwapchainImageViews)
			vkDestroyImageView(LogicalDevice, i, nullptr);

		vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
	}


	//Recrea el swapchain (al cambiar el tamaño de la ventana).
	void RecreateSwapchain() {
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(ventana, &width, &height);

		//Si está minimizado, esperar.
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(ventana, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(LogicalDevice);

		CloseSwapchain();

		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		if (GraphicsPipeline2D != nullptr)
			CreateGraphicsPipeline2D();
		if (GraphicsPipeline3D != nullptr)
			CreateGraphicsPipeline3D();
		CreateDepthResources();
		CreateFramebuffers();
	}


	//Obtiene el tipo de memoria indicado.
	uint32_t GetMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const {
		//Tipos de memoria disponibles.
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(GPU, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if (memoryTypeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags)
				return i;

		throw std::runtime_error("failed to find suitable memory type!");
	}


	//
	void Draw() {
		double oldTime = glfwGetTime();

		vkWaitForFences(LogicalDevice, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

		//Repreenta cual es la imagen que se va a renderizar.
		uint32_t imageID = 0;

		//Adquiere la siguiente imágen sobre la que se va a renderizar.
		VkResult result = vkAcquireNextImageKHR(LogicalDevice, Swapchain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageID);

		//La ventana ha cambiado de tamaño.
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();

			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("ERROR: obtener imagen.");

		if (ImagesInFlight[imageID] != nullptr)
			vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageID], VK_TRUE, UINT64_MAX);
		ImagesInFlight[imageID] = InFlightFences[CurrentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//Esperar a que la imagen esté disponible antes de renderizar.
		VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffers[imageID];

		//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(LogicalDevice, 1, &InFlightFences[CurrentFrame]);

		UpdateUBO(imageID);
		UpdateCommandBuffers();

		result = vkQueueSubmit(GraphicsQ, 1, &submitInfo, InFlightFences[CurrentFrame]);
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
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || HasFramebufferBeenResized) {
			RecreateSwapchain();
			HasFramebufferBeenResized = false;
		}

		double newTime = glfwGetTime();

		vkQueueWaitIdle(PresentQ);

		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		//std::cout << "FPS: " << 1 / (newTime - oldTime) << std::endl;

		std::string msOutput = "Render ms: " + std::to_string(newTime - oldTime) + "ms FPS: " + std::to_string(1 / (newTime - oldTime));
		glfwSetWindowTitle(ventana, msOutput.c_str());
	}


	//Crea un buffer.
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop, VulkanBuffer& buffer) {
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
		allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, prop);

		result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &buffer.Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc mem.");

		vkBindBufferMemory(LogicalDevice, buffer.Buffer, buffer.Memory, 0);
	}


	//Copia el contenido de un buffer a otro buffer.
	void CopyBuffer(VulkanBuffer source, VulkanBuffer destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;
		vkCmdCopyBuffer(cmdBuffer, source.Buffer, destination.Buffer, 1, &copyRegion);

		EndSingleTimeCommandBuffer(cmdBuffer);
	}


	//Destruye un buffer.
	void DestroyBuffer(VulkanBuffer& buffer) {
		vkDestroyBuffer(LogicalDevice, buffer.Buffer, nullptr);
		vkFreeMemory(LogicalDevice, buffer.Memory, nullptr);
	}

	void CreateDepthResources() {
		VkFormat depthFormat = GetDepthFormat();
		CreateImage(SwapchainExtent.width, SwapchainExtent.height, &DepthImage, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CreateImageView(&DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	bool FormatHasStencilComponent(VkFormat format) const {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkFormat GetDepthFormat() const {
		return GetSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat GetSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
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

	void CreateDescriptorSetLayout() {
		//UBO.
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		//__Texture.
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


	//Crea los buffers de UBO.
	void CreateUniformBuffers() {
		VkDeviceSize size = sizeof(UniformBufferObject);

		UniformBuffers.resize(SwapchainImages.size());

		for (uint32_t i = 0; i < UniformBuffers.size(); i++)
			CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffers[i]);
	}


	void CreateDescriptorPool() {
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
		poolInfo.maxSets = static_cast<uint32_t>(SwapchainImages.size() * MAX_TEXTURES);

		VkResult result = vkCreateDescriptorPool(LogicalDevice, &poolInfo, nullptr, &DescriptorPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor pool.");
	}


	void CreateDescriptorSets(VulkanObject* obj) {
		obj->DescriptorSets.resize(SwapchainImages.size());

		std::vector<VkDescriptorSetLayout> layouts(SwapchainImages.size(), DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(LogicalDevice, &allocInfo, obj->DescriptorSets.data());
		//if (result != VK_SUCCESS)
			//throw std::runtime_error("ERROR: crear descriptor sets.");

		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = UniformBuffers[i].Buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = obj->Texture->View;
			imageInfo.sampler = GlobalImageSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = obj->DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0; //Binding
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = obj->DescriptorSets[i];
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


	void CreateDescriptorSets() {
		if (DescriptorSets.size() != SwapchainImages.size())
			DescriptorSets.resize(SwapchainImages.size());

		std::vector<VkDescriptorSetLayout> layouts(SwapchainImages.size(), DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(LogicalDevice, &allocInfo, DescriptorSets.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor sets.");

		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = UniformBuffers[i].Buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture.View;
			imageInfo.sampler = GlobalImageSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0; //Binding
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = DescriptorSets[i];
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


	//Actualiza el UBO.
	void UpdateUBO(const uint32_t& currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		double oldTime = glfwGetTime();
		UniformBufferObject ubo{};
		ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.Projection = glm::perspective(glm::radians(45.0f), SwapchainExtent.width / (float)SwapchainExtent.height, 0.1f, 10.0f);
		ubo.Projection[1][1] *= -1;

		for (size_t i = 0; i < UniformBuffers.size(); i++) {
			void* data;
			vkMapMemory(LogicalDevice, UniformBuffers[i].Memory, 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(LogicalDevice, UniformBuffers[i].Memory);
		}
		Profiling.LastCopyUBOTime = glfwGetTime() - oldTime;
	}


	//Crea un módulo de shader.
	VkShaderModule GetShaderModule(const std::vector<char>& code) const {
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


	//Lee el contenido de un archivo.
	static std::vector<char> ReadFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("ERROR: abrir archivo.");

		const size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		//Volver al inicio.
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	//Comprueba la compatibilidad de la GPU.
	bool IsGPUsuitable(VkPhysicalDevice gpu) const {
		//Obtiene las propiedades de la gpu.
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

		//Obtiene las características de la GPU.
		VkPhysicalDeviceFeatures gpuFeatures;
		vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);

		//Comprobar soporte de colas.
		QueueFamilyIndices indices = FindQueueFamilies(gpu);

		if (!CheckGPUextensionSupport(gpu))
			return false;

		//Comprobar soporte de swapchain.
		bool swapchainSupported = false;

		SwapchainSupportDetails swapchainDetails = GetSwapchainSupportDetails(gpu);
		swapchainSupported = !swapchainDetails.PresentModes.empty() && !swapchainDetails.Formats.empty();

		std::cout << "GPU: " << gpuProperties.deviceName << std::endl;
		std::cout << "Drivers: " << gpuProperties.driverVersion << std::endl;
		std::cout << "Supported present modes: " << std::endl;
		for (const auto& i : swapchainDetails.PresentModes)
			std::cout << "\t" << ToString(i) << std::endl;

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

		return indices.IsComplete() && CheckGPUextensionSupport(gpu) && swapchainSupported && supportedFeatures.samplerAnisotropy;

		//Usar GPU dedicada.
		//return gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && gpuFeatures.geometryShader;
	}


	//Comprueba si la GPU soport estas extensiones.
	bool CheckGPUextensionSupport(VkPhysicalDevice gpu) const {
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


	//			OBJECTS			//		
	//Crea el vertex buffer de un objecto.
	void CreateVertexBuffer(VulkanObject* object) {
		VkDeviceSize size = sizeof(Vertex) * object->Vertices.size();

		//Buffer temporal CPU-GPU.
		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, object->Vertices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		//Buffer final en la GPU.
		CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, object->VertexBuffer);

		CopyBuffer(stagingBuffer, object->VertexBuffer, size);

		DestroyBuffer(stagingBuffer);
	}

	//Crea el index buffer de un objecto.
	void CreateIndexBuffer(VulkanObject* object) {
		VkDeviceSize size = sizeof(object->Indices[0]) * object->Indices.size();

		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, object->Indices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, object->IndexBuffer);
		CopyBuffer(stagingBuffer, object->IndexBuffer, size);

		DestroyBuffer(stagingBuffer);
	}

	//Actualiza el UBO.
	void UpdateObjectConstants(VulkanObject* object, float time, const uint32_t& index = 0) {
		//if (object->PushConstant.Is2D)
			//return;

		object->PushConstant.Model = glm::rotate(object->PushConstant.Model, glm::radians(time * 60.0f * (index + 1)), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	//							//	


	//Obtiene las colas disponibles en una GPU.
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice gpu) const {
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


	//Obtiene los detalles sobre el soporte de swapchain de la GPU.
	SwapchainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice gpu) const {
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


	//Obtiene el formato que se va a usar.
	VkSurfaceFormatKHR GetSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& formats) const {
		//Preferir sRGB.
		for (const auto& format : formats)
			if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
				return format;

		return formats[0];
	}


	//Obtiene el modo de presentación:
	//Cómo se cambian las imágenes del swapchain.
	//
	//	VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).
	//	VK_PRESENT_MODE_FIFO_KHR: VSync.
	//	VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.
	//	VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".
	VkPresentModeKHR GetPresentMode(const std::vector<VkPresentModeKHR>& modes) const {
		VkPresentModeKHR finalPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& mode : modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				finalPresentMode = mode;

		std::cout << "Present mode selected: " << ToString(finalPresentMode) << std::endl;

		return finalPresentMode;
	}


	//Obtiene el tamaño ideal del swapchain.
	VkExtent2D GetSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
		//Si está inicializado, devolver.
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		int width;
		int height;
		glfwGetFramebufferSize(ventana, &width, &height);

		VkExtent2D extent{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//Que no sea más grande de lo soportado.
		extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
		extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

		return extent;
	}


	//Consola para mostrar los mensajes de capas de validación.
#pragma region DebugConsole
	//Establece la consola para mostrar los mensajes de capas de validación.
	void SetupDebugConsole() {
#ifdef _DEBUG
		//Información de la consola.
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		GetDebugMessengerCreateInfo(createInfo);

		//Crearlo.
		VkResult result = CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugConsole);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: no se puede iniciar la consola de capas de validación.");
#endif
	}


	//Destruye la consola de capas devalidación.
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}


	//Carga la función necesaria para establecer la consola de capas de validación.
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}


	//Mensajes de validación antes de crear la instancia.
	void GetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}


	//Debug messages.
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		//Message severity:
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

		//Tipos de mensaje:
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

		std::cerr << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
#pragma endregion

	std::vector<VulkanObject> objects;

	//Ventana de GLFW.
	GLFWwindow* ventana = nullptr;


	//Instancia de Vulkan.
	VkInstance Instance;


	//GPU.
	VkPhysicalDevice GPU = nullptr;


	//Representación lógica de la GPU.
	VkDevice LogicalDevice;


	//Colas.
	//Para commandos gráficos, copia de buffers, etc.
	VkQueue GraphicsQ;
	//Para comandos que presentan la imagen en la superficie de la ventana.
	VkQueue PresentQ;

	bool Recreating = true;

	//Superficie sobre la cual se renderiza.
	VkSurfaceKHR Surface;


	//Swapchain.
	VkSwapchainKHR Swapchain;

	//Imágenes del swapchain.
	std::vector<VkImage> SwapchainImages;

	//ImageViews del swapchain (cómo acceder a las imágenes).
	std::vector<VkImageView> SwapchainImageViews;

	//Formato del swapchain.
	VkFormat SwapchainFormat;

	//Tamaño del swapchain.
	VkExtent2D SwapchainExtent;


	//Pipeline.
	VkPipeline GraphicsPipeline2D = nullptr;
	VkPipeline GraphicsPipeline3D = nullptr;

	//PipelineLayout: cómo le pasamos info a la GPU.
	VkPipelineLayout PipelineLayout2D;
	VkPipelineLayout PipelineLayout3D;

	//RenderPass.
	VkRenderPass RenderPass;


	//Framebuffers.
	std::vector<VkFramebuffer> Framebuffers;


	//Command pool.
	VkCommandPool CommandPool;

	//Command buffers (uno por cada imagen del swapchain).
	std::vector<VkCommandBuffer> CommandBuffers;


	//Sync.
	//GPU to CPU.
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;

	//CPU to GPU.
	std::vector<VkFence> InFlightFences;
	std::vector<VkFence> ImagesInFlight;

	uint32_t CurrentFrame = 0;

	bool UseDdepthRes = false;
	bool HasFramebufferBeenResized = false;

	struct {
		double LastUpdateCommandBuffersTime = 0.0f;
		double LastSubmitQueueTime = 0.0f;
		double LastPerObjectCommandBufferTime = 0.0f;
		double LastCopyUBOTime = 0.0f;
	} Profiling;

	//Buffers de UBO.
	//Almacenan los datos de UniformBufferObject.
	//Hay un buffer por cada imagen del swapchain.
	std::vector<VulkanBuffer> UniformBuffers;


	//UBO.
	VkDescriptorSetLayout DescriptorSetLayout;

	VkDescriptorPool DescriptorPool;

	std::vector<VkDescriptorSet> DescriptorSets;

	VkSampler GlobalImageSampler;


	VulkanImage DepthImage;

	//Uniform buffer object.
	//Almacena información que cambia una vez cada frame.
	//A esta información se accede desde los shaders.
	//Esta información es la misma durante todo el renderizado de un frame, no cambia de un objeto a otro.
	//Almacena información de la cámara.
	UniformBufferObject UBO{};


	//Callback al cambiar de tamaño la ventana.
	static void WindowResizeCallbac(GLFWwindow* window, int width, int height) {
		auto engine = reinterpret_cast<VulkanGame*>(glfwGetWindowUserPointer(window));
		engine->HasFramebufferBeenResized = true;
	}


	//Consola debug.
	//Nos permite imprimir mensajes de las capas de validación.
	VkDebugUtilsMessengerEXT DebugConsole;


	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	//Extensiones de la GPU que van a ser necesarias.
	const std::vector<const char*> gpuExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

};

#pragma endregion

#else

//Clase que usa Vulkan.
class VulkanGame {

public:

	void Run() {
		InitWindow();
		InitVulkan();
		MainLoop();
		Close();
	}

private:

	//Creamos la ventana.
	void InitWindow() {
		//Inicializar GLFW.
		glfwInit();

		//Para que no use OpenGL.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Creamos la ventana.
		ventana = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);

		//Callbacks.
		glfwSetWindowUserPointer(ventana, this);
		glfwSetFramebufferSizeCallback(ventana, WindowResizeCallbac);
	}


	//Inicializa Vulkan.
	void InitVulkan(const RenderMode& renderMode) {
		CreateInstance();
		SetupDebugConsole();
		CreateSurface();
		GetPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();

		CreateCommandPool();
		CreateDepthResources();
		CreateFramebuffers();
		CreateGlobalImageSampler();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateSyncObjects();

		CreateTextureImage(&texture, "Img/td.png");

		VulkanObject obj{};
		obj.Texture = &texture;
		objects.push_back(obj);

		obj.Vertices = {
			{{-1.5f, -0.25f, 0}, {1.0f, 0.0f, 1.0f}, {2.0f, 0.0f}},
			{{1.5f, -0.25f, 0}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.5f, 0.25f, 0}, {1.0f, 0.0f, 1.0f}, {0.0f, 2.0f}},
			{{-1.5f, 0.25f, 0}, {1.0f, 1.0f, 1.0f}, {2.0f, 2.0f}}
		};
		//objects.push_back(obj);

		for (auto& i : objects) {
			CreateVertexBuffer(&i);
			CreateIndexBuffer(&i);
			if (i.Texture == nullptr)
				i.Texture = &texture;
			CreateDescriptorSets(&i);
		}

		CreateVertexBuffer();

		CreateCommandBuffers();
	}

	int ispressed = false;
	void MainLoop() {
		double lastTime = glfwGetTime();
		double currTime = lastTime;
		while (!glfwWindowShouldClose(ventana)) {
			glfwPollEvents();
			currTime = glfwGetTime();

			for (int i = 0; i < objects.size(); i++)
				UpdateObjectConstants(&objects[i], currTime - lastTime, i);

			lastTime = glfwGetTime();
			Draw();

			if (!ispressed && glfwGetKey(ventana, GLFW_KEY_ENTER) == GLFW_PRESS)
				ShowProfilingResults();
			ispressed = glfwGetKey(ventana, GLFW_KEY_ENTER);
		}

		vkDeviceWaitIdle(LogicalDevice);
	}


	void Close() {
		CloseSwapchain();

		DestroyImage(texture);
		vkDestroySampler(LogicalDevice, GlobalImageSampler, nullptr);

		for (auto& i : UniformBuffers)
			DestroyBuffer(i);

		vkDestroyDescriptorSetLayout(LogicalDevice, DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(LogicalDevice, DescriptorPool, nullptr);

		for (auto& i : objects) {
			DestroyBuffer(i.VertexBuffer);
			DestroyBuffer(i.IndexBuffer);
		}

		DestroySyncObjects();

		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

		DestroyLogicalDevice();
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(Instance, DebugConsole, nullptr);
#endif
		DestroySurface();

		//Destruir la instancia.
		vkDestroyInstance(Instance, nullptr);

		//Destruir la ventana.
		glfwDestroyWindow(ventana);
		glfwTerminate();
	}


	//Crea la instancia de Vulkan.
	void CreateInstance() {
		//Inicializar capas de validación.
#ifdef _DEBUG
		if (!CheckValidationLayerSupport())
			throw new std::runtime_error("ERROR: no se pueden iniciar las capas de validación.");
#endif

		//Información de la app.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "OSKengine Vk";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		//Create info.
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//Establecemos las extensiones.
		auto extensions = GetExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

#ifdef _DEBUG
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugConsoleCreateInfo{};
		GetDebugMessengerCreateInfo(debugConsoleCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugConsoleCreateInfo;
#endif

		//Crear la instancia y error-handling.
		VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);

		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear instancia de Vulkan.");

		//Output.
#ifdef _DEBUG
		std::cout << "Extensiones: " << std::endl;
		for (const auto& e : extensions)
			std::cout << e << std::endl;
#endif

	}

	//Comprueba si se pueden usar capas de validación.
	bool CheckValidationLayerSupport() {
		//Número de capas.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		//Obtener las capas disponibles.
		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		//Comprobación
		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : layers) {
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


	//Extensiones a usar.
	std::vector<const char*> GetExtensions() const {
		//Extensiones de la ventana.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		//Extensiones totales.
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}


	//Obtiene la GPU.
	void GetPhysicalDevice() {
		//Obtiene el número de GPUs disponibles.
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(Instance, &count, nullptr);

		if (count == 0)
			throw std::runtime_error("ERROR: no se encuentra ninguna GPU compatible.");

		//Obtiene las GPUs.
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(Instance, &count, devices.data());

		//Comprobar la compatibilidad de las GPUs.
		//Obtener una GPU compatible.
		for (const auto& gpu : devices) {
			if (IsGPUsuitable(gpu)) {
				GPU = gpu;

				break;
			}
		}

		if (GPU == nullptr)
			throw std::runtime_error("ERROR: no se encuentra ninguna GPU compatible.");
	}


	//Crea el logical device.
	void CreateLogicalDevice() {
		//Establecer las colas que vamos a usar.
		QueueFamilyIndices indices = FindQueueFamilies(GPU);

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


	//Destruye el logical device.
	void DestroyLogicalDevice() {
		vkDestroyDevice(LogicalDevice, nullptr);
	}


	//Crea la superficie.
	void CreateSurface() {
		//Obtener la superficie.
		VkResult result = glfwCreateWindowSurface(Instance, ventana, nullptr, &Surface);

		//Error-handling.
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear superficie.");

	}


	//Destruye la superficie.
	void DestroySurface() {
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
	}


	//Crea el swapchain.
	void CreateSwapchain() {
		//Soporte del swapchain.
		SwapchainSupportDetails swapchainDetails = GetSwapchainSupportDetails(GPU);

		//Formato del swapchain.
		VkSurfaceFormatKHR surfaceFormat = GetSwapchainFormat(swapchainDetails.Formats);

		//Modo de pressentación.
		VkPresentModeKHR presentMode = GetPresentMode(swapchainDetails.PresentModes);

		//tamaño.
		VkExtent2D extent = GetSwapchainExtent(swapchainDetails.SurfaceCapabilities);

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
		const QueueFamilyIndices indices = FindQueueFamilies(GPU);
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


	//Crea los image views del swapchain.
	void CreateSwapchainImageViews() {
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


	//Crea el graphics pipeline.
	void CreateGraphicsPipeline3D() {
		//Código de los shaders.
		const std::vector<char> vertexCode = ReadFile("Shaders/vert.spv");
		const std::vector<char> fragmentCode = ReadFile("Shaders/frag.spv");

		//Shader modules.
		VkShaderModule vertexModule = GetShaderModule(vertexCode);
		VkShaderModule fragmentModule = GetShaderModule(fragmentCode);

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
		const auto attribDesc = Vertex::GetAttributeDescriptions_FullVertex();

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
		pushConstRange.size = sizeof(PushConst);

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
		pipelineInfo.renderPass = RenderPass;
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


	//Crea el graphics pipeline.
	void CreateGraphicsPipeline2D() {
		//Código de los shaders.
		const std::vector<char> vertexCode = ReadFile("Shaders/2D/vert.spv");
		const std::vector<char> fragmentCode = ReadFile("Shaders/2D/frag.spv");

		//Shader modules.
		VkShaderModule vertexModule = GetShaderModule(vertexCode);
		VkShaderModule fragmentModule = GetShaderModule(fragmentCode);

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
		const auto attribDesc = Vertex::GetAttributeDescriptions_FullVertex();

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
		pushConstRange.size = sizeof(PushConst);

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
		pipelineInfo.renderPass = RenderPass;
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


	//Crea un renderpass.
	void CreateRenderPass() {
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
		depthAttachment.format = GetDepthFormat();
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

		VkResult result = vkCreateRenderPass(LogicalDevice, &renderPassInfo, nullptr, &RenderPass);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear renderpass.");
	}


	//Crea los framebuffers.
	void CreateFramebuffers() {
		Framebuffers.resize(SwapchainImageViews.size());

		//Crear los framebuffers.
		for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments = {
				SwapchainImageViews[i],
				DepthImage.View
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = RenderPass;
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


	//Crea el command pool.
	void CreateCommandPool() {
		QueueFamilyIndices indices = FindQueueFamilies(GPU);

		//Para la graphics q.
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult result = vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &CommandPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear command pool.");
	}


	//Crea los command buffers.
	void CreateCommandBuffers() {
		if (CommandBuffers.size() != Framebuffers.size())
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


	void CreateTextureImage(VulkanImage* img, const std::string& path) {
		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
		VkDeviceSize size = width * height * 4;

		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);
		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(size));
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		stbi_image_free(pixels);
		CreateImage(width, height, img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransitionImageLayout(img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, img, width, height);
		TransitionImageLayout(img, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		DestroyBuffer(stagingBuffer);

		CreateImageView(img, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void CreateImageView(VulkanImage* img, VkFormat format, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT) const {
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

	void CreateGlobalImageSampler() {
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

	VulkanImage texture;

	void CreateImage(int width, int height, VulkanImage* image, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
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
		allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &image->Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc memoria de la imagen.");

		vkBindImageMemory(LogicalDevice, image->Image, image->Memory, 0);
	}

	void UpdateCommandBuffers() {
		//Grabar los comandos.
		double oldTime = glfwGetTime();
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
			renderPassInfo.renderPass = RenderPass;
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
			vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline3D);

			VulkanImage* lastImg = nullptr;
			for (const auto& obj : objects) {
				double time = glfwGetTime();

				VkBuffer vertexBuffers[] = { obj.VertexBuffer.Buffer };
				VkDeviceSize offsets[] = { 0 };
				if (obj.Texture != lastImg) {
					vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout3D, 0, 1, &obj.DescriptorSets[i], 0, nullptr);
					lastImg = obj.Texture;
				}
				vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(CommandBuffers[i], obj.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdPushConstants(CommandBuffers[i], PipelineLayout3D, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst), &obj.PushConstant);
				vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(obj.Indices.size()), 1, 0, 0, 0);

				Profiling.LastPerObjectCommandBufferTime = glfwGetTime() - time;
			}

			vkCmdEndRenderPass(CommandBuffers[i]);
			result = vkEndCommandBuffer(CommandBuffers[i]);
			if (result != VK_SUCCESS)
				throw std::runtime_error("ERROR: grabar renderpass.");
		}

		Profiling.LastUpdateCommandBuffersTime = glfwGetTime() - oldTime;
	}

	void ShowProfilingResults() {
		std::cout << "UpdateCommandBuffers(" << CommandBuffers.size() << "): " << Profiling.LastUpdateCommandBuffersTime << std::endl;
		std::cout << "Single object draw call: " << Profiling.LastPerObjectCommandBufferTime << std::endl;
		std::cout << "Copy UBO: " << Profiling.LastCopyUBOTime << std::endl;
	}

	void DestroyImage(VulkanImage& img) const {
		vkDestroyImageView(LogicalDevice, img.View, nullptr);
		vkDestroyImage(LogicalDevice, img.Image, nullptr);
		vkFreeMemory(LogicalDevice, img.Memory, nullptr);
	}

	VkCommandBuffer BeginSingleTimeCommandBuffer() const {
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

	//Cambiar el layout de la imagen, si fuera necesario.
	void TransitionImageLayout(VulkanImage* img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

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

		EndSingleTimeCommandBuffer(cmdBuffer);
	}

	void CopyBufferToImage(const VulkanBuffer& buffer, VulkanImage* img, const uint32_t& width, const uint32_t& height) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

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

		vkCmdCopyBufferToImage(cmdBuffer, buffer.Buffer, img->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommandBuffer(cmdBuffer);
	}

	void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) const {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQ, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQ);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &commandBuffer);
	}

	//Establece las variables de sincronización CPU-GPU.
	void CreateSyncObjects() {
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

	void DestroySyncObjects() {
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);

			InFlightFences[i] = nullptr;
		}
	}

	//
	void CloseSwapchain() {
		DestroyImage(DepthImage);

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

		vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

		for (const auto& i : SwapchainImageViews)
			vkDestroyImageView(LogicalDevice, i, nullptr);

		vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
	}


	//Recrea el swapchain (al cambiar el tamaño de la ventana).
	void RecreateSwapchain() {
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(ventana, &width, &height);

		//Si está minimizado, esperar.
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(ventana, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(LogicalDevice);

		CloseSwapchain();

		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		if (GraphicsPipeline2D != nullptr)
			CreateGraphicsPipeline2D();
		if (GraphicsPipeline3D != nullptr)
			CreateGraphicsPipeline3D();
		CreateDepthResources();
		CreateFramebuffers();
	}


	//Obtiene el tipo de memoria indicado.
	uint32_t GetMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const {
		//Tipos de memoria disponibles.
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(GPU, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if (memoryTypeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags)
				return i;

		throw std::runtime_error("failed to find suitable memory type!");
	}


	//
	void Draw() {
		double oldTime = glfwGetTime();

		vkWaitForFences(LogicalDevice, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

		//Repreenta cual es la imagen que se va a renderizar.
		uint32_t imageID = 0;

		//Adquiere la siguiente imágen sobre la que se va a renderizar.
		VkResult result = vkAcquireNextImageKHR(LogicalDevice, Swapchain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageID);

		//La ventana ha cambiado de tamaño.
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();

			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("ERROR: obtener imagen.");

		if (ImagesInFlight[imageID] != nullptr)
			vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageID], VK_TRUE, UINT64_MAX);
		ImagesInFlight[imageID] = InFlightFences[CurrentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//Esperar a que la imagen esté disponible antes de renderizar.
		VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffers[imageID];

		//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(LogicalDevice, 1, &InFlightFences[CurrentFrame]);

		UpdateUBO(imageID);
		UpdateCommandBuffers();

		result = vkQueueSubmit(GraphicsQ, 1, &submitInfo, InFlightFences[CurrentFrame]);
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
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || HasFramebufferBeenResized) {
			RecreateSwapchain();
			HasFramebufferBeenResized = false;
		}

		double newTime = glfwGetTime();

		vkQueueWaitIdle(PresentQ);

		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		//std::cout << "FPS: " << 1 / (newTime - oldTime) << std::endl;

		std::string msOutput = "Render ms: " + std::to_string(newTime - oldTime) + "ms FPS: " + std::to_string(1 / (newTime - oldTime));
		glfwSetWindowTitle(ventana, msOutput.c_str());
	}


	//Crea un buffer.
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop, VulkanBuffer& buffer) {
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
		allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, prop);

		result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &buffer.Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc mem.");

		vkBindBufferMemory(LogicalDevice, buffer.Buffer, buffer.Memory, 0);
	}


	//Copia el contenido de un buffer a otro buffer.
	void CopyBuffer(VulkanBuffer source, VulkanBuffer destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const {
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommandBuffer();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;
		vkCmdCopyBuffer(cmdBuffer, source.Buffer, destination.Buffer, 1, &copyRegion);

		EndSingleTimeCommandBuffer(cmdBuffer);
	}


	//Destruye un buffer.
	void DestroyBuffer(VulkanBuffer& buffer) {
		vkDestroyBuffer(LogicalDevice, buffer.Buffer, nullptr);
		vkFreeMemory(LogicalDevice, buffer.Memory, nullptr);
	}

	void CreateDepthResources() {
		VkFormat depthFormat = GetDepthFormat();
		CreateImage(SwapchainExtent.width, SwapchainExtent.height, &DepthImage, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CreateImageView(&DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	bool FormatHasStencilComponent(VkFormat format) const {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkFormat GetDepthFormat() const {
		return GetSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat GetSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
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

	void CreateDescriptorSetLayout() {
		//UBO.
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		//__Texture.
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


	//Crea los buffers de UBO.
	void CreateUniformBuffers() {
		VkDeviceSize size = sizeof(UniformBufferObject);

		UniformBuffers.resize(SwapchainImages.size());

		for (uint32_t i = 0; i < UniformBuffers.size(); i++)
			CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffers[i]);
	}


	void CreateDescriptorPool() {
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
		poolInfo.maxSets = static_cast<uint32_t>(SwapchainImages.size() * MAX_TEXTURES);

		VkResult result = vkCreateDescriptorPool(LogicalDevice, &poolInfo, nullptr, &DescriptorPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor pool.");
	}


	void CreateDescriptorSets(VulkanObject* obj) {
		obj->DescriptorSets.resize(SwapchainImages.size());

		std::vector<VkDescriptorSetLayout> layouts(SwapchainImages.size(), DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(LogicalDevice, &allocInfo, obj->DescriptorSets.data());
		//if (result != VK_SUCCESS)
			//throw std::runtime_error("ERROR: crear descriptor sets.");

		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = UniformBuffers[i].Buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = obj->Texture->View;
			imageInfo.sampler = GlobalImageSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = obj->DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0; //Binding
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = obj->DescriptorSets[i];
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


	void CreateDescriptorSets() {
		if (DescriptorSets.size() != SwapchainImages.size())
			DescriptorSets.resize(SwapchainImages.size());

		std::vector<VkDescriptorSetLayout> layouts(SwapchainImages.size(), DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(LogicalDevice, &allocInfo, DescriptorSets.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor sets.");

		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = UniformBuffers[i].Buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture.View;
			imageInfo.sampler = GlobalImageSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0; //Binding
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = DescriptorSets[i];
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


	//Actualiza el UBO.
	void UpdateUBO(const uint32_t& currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		double oldTime = glfwGetTime();
		UniformBufferObject ubo{};
		ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.Projection = glm::perspective(glm::radians(45.0f), SwapchainExtent.width / (float)SwapchainExtent.height, 0.1f, 10.0f);
		ubo.Projection[1][1] *= -1;

		for (size_t i = 0; i < UniformBuffers.size(); i++) {
			void* data;
			vkMapMemory(LogicalDevice, UniformBuffers[i].Memory, 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(LogicalDevice, UniformBuffers[i].Memory);
		}
		Profiling.LastCopyUBOTime = glfwGetTime() - oldTime;
	}


	//Crea un módulo de shader.
	VkShaderModule GetShaderModule(const std::vector<char>& code) const {
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


	//Lee el contenido de un archivo.
	static std::vector<char> ReadFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("ERROR: abrir archivo.");

		const size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		//Volver al inicio.
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	//Comprueba la compatibilidad de la GPU.
	bool IsGPUsuitable(VkPhysicalDevice gpu) const {
		//Obtiene las propiedades de la gpu.
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

		//Obtiene las características de la GPU.
		VkPhysicalDeviceFeatures gpuFeatures;
		vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);

		//Comprobar soporte de colas.
		QueueFamilyIndices indices = FindQueueFamilies(gpu);

		if (!CheckGPUextensionSupport(gpu))
			return false;

		//Comprobar soporte de swapchain.
		bool swapchainSupported = false;

		SwapchainSupportDetails swapchainDetails = GetSwapchainSupportDetails(gpu);
		swapchainSupported = !swapchainDetails.PresentModes.empty() && !swapchainDetails.Formats.empty();

		std::cout << "GPU: " << gpuProperties.deviceName << std::endl;
		std::cout << "Drivers: " << gpuProperties.driverVersion << std::endl;
		std::cout << "Supported present modes: " << std::endl;
		for (const auto& i : swapchainDetails.PresentModes)
			std::cout << "\t" << ToString(i) << std::endl;

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

		return indices.IsComplete() && CheckGPUextensionSupport(gpu) && swapchainSupported && supportedFeatures.samplerAnisotropy;

		//Usar GPU dedicada.
		//return gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && gpuFeatures.geometryShader;
	}


	//Comprueba si la GPU soport estas extensiones.
	bool CheckGPUextensionSupport(VkPhysicalDevice gpu) const {
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


	//			OBJECTS			//		
	//Crea el vertex buffer de un objecto.
	void CreateVertexBuffer(VulkanObject* object) {
		VkDeviceSize size = sizeof(Vertex) * object->Vertices.size();

		//Buffer temporal CPU-GPU.
		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, object->Vertices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		//Buffer final en la GPU.
		CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, object->VertexBuffer);

		CopyBuffer(stagingBuffer, object->VertexBuffer, size);

		DestroyBuffer(stagingBuffer);
	}

	//Crea el index buffer de un objecto.
	void CreateIndexBuffer(VulkanObject* object) {
		VkDeviceSize size = sizeof(object->Indices[0]) * object->Indices.size();

		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, object->Indices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, object->IndexBuffer);
		CopyBuffer(stagingBuffer, object->IndexBuffer, size);

		DestroyBuffer(stagingBuffer);
	}

	//Actualiza el UBO.
	void UpdateObjectConstants(VulkanObject* object, float time, const uint32_t& index = 0) {
		//if (object->PushConstant.Is2D)
			//return;

		object->PushConstant.Model = glm::rotate(object->PushConstant.Model, glm::radians(time * 60.0f * (index + 1)), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	//							//	


	//Obtiene las colas disponibles en una GPU.
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice gpu) const {
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


	//Obtiene los detalles sobre el soporte de swapchain de la GPU.
	SwapchainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice gpu) const {
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


	//Obtiene el formato que se va a usar.
	VkSurfaceFormatKHR GetSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& formats) const {
		//Preferir sRGB.
		for (const auto& format : formats)
			if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
				return format;

		return formats[0];
	}


	//Obtiene el modo de presentación:
	//Cómo se cambian las imágenes del swapchain.
	//
	//	VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).
	//	VK_PRESENT_MODE_FIFO_KHR: VSync.
	//	VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.
	//	VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".
	VkPresentModeKHR GetPresentMode(const std::vector<VkPresentModeKHR>& modes) const {
		VkPresentModeKHR finalPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& mode : modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				finalPresentMode = mode;

		std::cout << "Present mode selected: " << ToString(finalPresentMode) << std::endl;

		return finalPresentMode;
	}


	//Obtiene el tamaño ideal del swapchain.
	VkExtent2D GetSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
		//Si está inicializado, devolver.
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		int width;
		int height;
		glfwGetFramebufferSize(ventana, &width, &height);

		VkExtent2D extent{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//Que no sea más grande de lo soportado.
		extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
		extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

		return extent;
	}


	//Consola para mostrar los mensajes de capas de validación.
#pragma region DebugConsole
	//Establece la consola para mostrar los mensajes de capas de validación.
	void SetupDebugConsole() {
#ifdef _DEBUG
		//Información de la consola.
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		GetDebugMessengerCreateInfo(createInfo);

		//Crearlo.
		VkResult result = CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugConsole);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: no se puede iniciar la consola de capas de validación.");
#endif
	}


	//Destruye la consola de capas devalidación.
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}


	//Carga la función necesaria para establecer la consola de capas de validación.
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}


	//Mensajes de validación antes de crear la instancia.
	void GetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}


	//Debug messages.
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		//Message severity:
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
		//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

		//Tipos de mensaje:
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
		//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

		std::cerr << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
#pragma endregion

	std::vector<VulkanObject> objects;

	//Ventana de GLFW.
	GLFWwindow* ventana = nullptr;


	//Instancia de Vulkan.
	VkInstance Instance;


	//GPU.
	VkPhysicalDevice GPU = nullptr;


	//Representación lógica de la GPU.
	VkDevice LogicalDevice;


	//Colas.
	//Para commandos gráficos, copia de buffers, etc.
	VkQueue GraphicsQ;
	//Para comandos que presentan la imagen en la superficie de la ventana.
	VkQueue PresentQ;

	bool Recreating = true;

	//Superficie sobre la cual se renderiza.
	VkSurfaceKHR Surface;


	//Swapchain.
	VkSwapchainKHR Swapchain;

	//Imágenes del swapchain.
	std::vector<VkImage> SwapchainImages;

	//ImageViews del swapchain (cómo acceder a las imágenes).
	std::vector<VkImageView> SwapchainImageViews;

	//Formato del swapchain.
	VkFormat SwapchainFormat;

	//Tamaño del swapchain.
	VkExtent2D SwapchainExtent;


	//Pipeline.
	VkPipeline GraphicsPipeline2D = nullptr;
	VkPipeline GraphicsPipeline3D = nullptr;

	//PipelineLayout: cómo le pasamos info a la GPU.
	VkPipelineLayout PipelineLayout2D;
	VkPipelineLayout PipelineLayout3D;

	//RenderPass.
	VkRenderPass RenderPass;


	//Framebuffers.
	std::vector<VkFramebuffer> Framebuffers;


	//Command pool.
	VkCommandPool CommandPool;

	//Command buffers (uno por cada imagen del swapchain).
	std::vector<VkCommandBuffer> CommandBuffers;


	//Sync.
	//GPU to CPU.
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;

	//CPU to GPU.
	std::vector<VkFence> InFlightFences;
	std::vector<VkFence> ImagesInFlight;

	uint32_t CurrentFrame = 0;

	bool UseDdepthRes = false;
	bool HasFramebufferBeenResized = false;

	struct {
		double LastUpdateCommandBuffersTime = 0.0f;
		double LastSubmitQueueTime = 0.0f;
		double LastPerObjectCommandBufferTime = 0.0f;
		double LastCopyUBOTime = 0.0f;
	} Profiling;

	//Buffers de UBO.
	//Almacenan los datos de UniformBufferObject.
	//Hay un buffer por cada imagen del swapchain.
	std::vector<VulkanBuffer> UniformBuffers;


	//UBO.
	VkDescriptorSetLayout DescriptorSetLayout;

	VkDescriptorPool DescriptorPool;

	std::vector<VkDescriptorSet> DescriptorSets;

	VkSampler GlobalImageSampler;


	VulkanImage DepthImage;

	//Uniform buffer object.
	//Almacena información que cambia una vez cada frame.
	//A esta información se accede desde los shaders.
	//Esta información es la misma durante todo el renderizado de un frame, no cambia de un objeto a otro.
	//Almacena información de la cámara.
	UniformBufferObject UBO{};


	//Callback al cambiar de tamaño la ventana.
	static void WindowResizeCallbac(GLFWwindow* window, int width, int height) {
		auto engine = reinterpret_cast<VulkanGame*>(glfwGetWindowUserPointer(window));
		engine->HasFramebufferBeenResized = true;
	}


	//Consola debug.
	//Nos permite imprimir mensajes de las capas de validación.
	VkDebugUtilsMessengerEXT DebugConsole;


	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	//Extensiones de la GPU que van a ser necesarias.
	const std::vector<const char*> gpuExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const float vertices[9] = {
		0.25f, 0.25f, 0.0f,
		0.75f, 0.25f, 0.0f,
		0.50f, 0.75f, 0.0f
	};

	const uint32_t indices[3] = { 0, 1, 2 };

};

#endif

int main() {

	VulkanGame game;
	
	try {
		game.Run(RenderMode::RENDER_MODE_2D_AND_3D);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;

		return -1;
	}

	return 0;
}