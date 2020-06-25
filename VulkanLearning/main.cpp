#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

#include "QueueFamilyIndices.hpp"
#include "SwapchainSupportDetails.hpp"
#include "Vertex.hpp"
#include "VulkanBuffer.h"
#include "UBO.h"
#include "Object.hpp"

#include <set>
#include <algorithm>
#include <fstream>
#include <string>

//Tamaño inicial de la ventana.
constexpr int32_t INIT_WINDOW_WIDTH = 800;
constexpr int32_t INIT_WINDOW_HEIGHT = 600;

//Máximo de frames renderizados.
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

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
		//TODO: resize support.
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//Creamos la ventana.
		ventana = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);

		//Callbacks.
		glfwSetWindowUserPointer(ventana, this);
		glfwSetFramebufferSizeCallback(ventana, WindowResizeCallbac);
	}


	//Inicializa Vulkan.
	void InitVulkan() {
		CreateInstance();
		SetupDebugConsole();
		CreateSurface();
		GetPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();
	}


	void MainLoop() {
		while (!glfwWindowShouldClose(ventana)) {
			glfwPollEvents();
			Draw();
		}

		vkDeviceWaitIdle(LogicalDevice);
	}


	void Close() {
		CloseSwapchain();

		for (auto& i : UniformBuffers)
			DestroyBuffer(i);

		vkDestroyDescriptorSetLayout(LogicalDevice, DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(LogicalDevice, DescriptorPool, nullptr);

		DestroyBuffer(VertexBuffer);
		DestroyBuffer(IndexBuffer);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
		}

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
		VkSwapchainCreateInfoKHR createInfo {};
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
	void CreateGraphicsPipeline() {
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

		//PipelineLayout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr; 

		VkResult result = vkCreatePipelineLayout(LogicalDevice, &pipelineLayoutInfo, nullptr, &PipelineLayout);
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
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = PipelineLayout;
		pipelineInfo.renderPass = RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr; 
		pipelineInfo.basePipelineIndex = -1;

		result = vkCreateGraphicsPipelines(LogicalDevice, nullptr, 1, &pipelineInfo, nullptr, &GraphicsPipeline);
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

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
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
			VkImageView attachments[] = {
				SwapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = RenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
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
		poolInfo.flags = 0;

		VkResult result = vkCreateCommandPool(LogicalDevice, &poolInfo, nullptr, &CommandPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear command pool.");
	}


	//Crea los command buffers.
	void CreateCommandBuffers() {
		CommandBuffers.resize(Framebuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(LogicalDevice, &allocInfo, CommandBuffers.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear command buffers.");

		//Grabar los comandos.
		for (size_t i = 0; i < CommandBuffers.size(); i++) {
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
			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			//Comenzar el renderizado.
			vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
			
			//Vertex data:
			VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(CommandBuffers[i], IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

			UpdateUBO(0);
			vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[i], 0, nullptr);
			vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(CommandBuffers[i]);
			result = vkEndCommandBuffer(CommandBuffers[i]);
			if (result != VK_SUCCESS)
				throw std::runtime_error("ERROR: grabar renderpass.");
		}
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
		}
	}


	//
	void CloseSwapchain() {
		for (const auto& i : Framebuffers)
			vkDestroyFramebuffer(LogicalDevice, i, nullptr);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());

		vkDestroyPipeline(LogicalDevice, GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(LogicalDevice, PipelineLayout, nullptr);
		vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

		for (const auto& i : SwapchainImageViews)
			vkDestroyImageView(LogicalDevice, i, nullptr);

		vkDestroySwapchainKHR(LogicalDevice, Swapchain, nullptr);
	}


	//Recrea el swapchain (al cambiar el tamaño de la ventana).
	void RecreateSwapchain() {
		//Si está minimizado, esperar.
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(ventana, &width, &height);

		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(ventana, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(LogicalDevice);

		CloseSwapchain();

		CreateSwapchain();
		CreateSwapchainImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
	}


	//Crea el vertex buffer.
	void CreateVertexBuffer() {
		VkDeviceSize size = sizeof(Vertex) * Vertices.size();
		
		//Buffer temporal CPU-GPU.
		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, Vertices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		//Buffer final en la GPU.
		CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexBuffer);

		CopyBuffer(stagingBuffer, VertexBuffer, size);

		DestroyBuffer(stagingBuffer);
	}


	void CreateIndexBuffer() {
		VkDeviceSize size = sizeof(Indices[0]) * Indices.size();

		VulkanBuffer stagingBuffer;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

		void* data;
		vkMapMemory(LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, Indices.data(), (size_t)size);
		vkUnmapMemory(LogicalDevice, stagingBuffer.Memory);

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer);
		CopyBuffer(stagingBuffer, IndexBuffer, size);

		DestroyBuffer(stagingBuffer);
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
		//Repreenta cual es la imagen que se va a renderizar.
		uint32_t imageID = 0;

		//Adquiere la siguiente imágen sobre la que se va a renderizar.
		VkResult result = vkAcquireNextImageKHR(LogicalDevice, Swapchain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageID);

		//double oldTime = glfwGetTime();

		//La ventana ha cambiado de tamaño.
		if (result == VK_ERROR_OUT_OF_DATE_KHR || HasFramebufferBeenResized) {
			RecreateSwapchain();
			HasFramebufferBeenResized = false;

			return;
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: obtener imagen.");

		if (ImagesInFlight[imageID] != nullptr)
			vkWaitForFences(LogicalDevice, 1, &ImagesInFlight[imageID], VK_TRUE, UINT64_MAX);
		ImagesInFlight[imageID] = InFlightFences[CurrentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		UpdateUBO(imageID);

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

		vkQueueWaitIdle(PresentQ);

		//double newTime = glfwGetTime();

		//glfwSetWindowTitle(ventana, std::to_string(1 / (newTime - oldTime)).c_str());

		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		//std::string msOutput = "Render ms: " + std::to_string(currentTime - previousTime) + "ms";
		//glfwSetWindowTitle(ventana, msOutput.c_str());
	}


	//Crea un buffer..
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
		//Comando para copiar el buffer.
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

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, source.Buffer, destination.Buffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQ, 1, &submitInfo, nullptr);
		vkQueueWaitIdle(GraphicsQ);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &commandBuffer);
	}


	//Destruye un buffer.
	void DestroyBuffer(VulkanBuffer& buffer) {
		vkDestroyBuffer(LogicalDevice, buffer.Buffer, nullptr);
		vkFreeMemory(LogicalDevice, buffer.Memory, nullptr);
	}


	void CreateDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

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
		//Tamaño del pool (número de descritors).
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(SwapchainImages.size());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(SwapchainImages.size());

		VkResult result = vkCreateDescriptorPool(LogicalDevice, &poolInfo, nullptr, &DescriptorPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor pool.");
	}


	void CreateDescriptorSets() {
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

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = DescriptorSets[i];
			descriptorWrite.dstBinding = 0; //Binding
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(LogicalDevice, 1, &descriptorWrite, 0, nullptr);
		}
	}


	//Actualiza el UBO.
	void UpdateUBO(const uint32_t& currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.Projection = glm::perspective(glm::radians(45.0f), SwapchainExtent.width / (float)SwapchainExtent.height, 0.1f, 10.0f);
		ubo.Projection[1][1] *= -1;

		void* data;
		vkMapMemory(LogicalDevice, UniformBuffers[currentImage].Memory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(LogicalDevice, UniformBuffers[currentImage].Memory);
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

		return indices.IsComplete() && CheckGPUextensionSupport(gpu) && swapchainSupported;

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


	/*			OBJECTS			*/		
	void UpdateCommandBuffers(VulkanObject* object) {
		if (object->CommandBuffers.size() != Framebuffers.size())
		CommandBuffers.resize(Framebuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(LogicalDevice, &allocInfo, CommandBuffers.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear command buffers.");

		//Grabar los comandos.
		for (size_t i = 0; i < CommandBuffers.size(); i++) {
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
			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			//Comenzar el renderizado.
			vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

			//Vertex data:
			VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(CommandBuffers[i], IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[i], 0, nullptr);

			vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

			UpdateUBO(0);
			vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[i], 0, nullptr);
			vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(CommandBuffers[i]);
			result = vkEndCommandBuffer(CommandBuffers[i]);
			if (result != VK_SUCCESS)
				throw std::runtime_error("ERROR: grabar renderpass.");
		}
	}
	/*							*/	

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
		//return VK_PRESENT_MODE_IMMEDIATE_KHR;
		//Preferir triple buffer.
		for (const auto& mode : modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;

		//Siempre va a estar soportado.
		return VK_PRESENT_MODE_FIFO_KHR;
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


	//Ventana de GLFW.
	GLFWwindow* ventana = nullptr;


	//Instancia de Vulkan.
	VkInstance Instance;


	//GPU.
	VkPhysicalDevice GPU = nullptr;


	//Representación de la GPU.
	VkDevice LogicalDevice;


	//Colas.
	VkQueue GraphicsQ;
	VkQueue PresentQ;


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
	VkPipeline GraphicsPipeline;

	//PipelineLayout: cómo le pasamos info a la GPU.
	VkPipelineLayout PipelineLayout;

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


	bool HasFramebufferBeenResized = false;


	//Buffer de los vértices.
	VulkanBuffer VertexBuffer;
	VulkanBuffer IndexBuffer;


	//Buffers de UBO.
	std::vector<VulkanBuffer> UniformBuffers;


	//UBO.
	VkDescriptorSetLayout DescriptorSetLayout;

	VkDescriptorPool DescriptorPool;

	std::vector<VkDescriptorSet> DescriptorSets;


	UniformBufferObject UBO{};


	//Callback al cambiar de tamaño la ventana.
	static void WindowResizeCallbac(GLFWwindow* window, int width, int height) {
		auto engine = reinterpret_cast<VulkanGame*>(glfwGetWindowUserPointer(window));
		engine->HasFramebufferBeenResized = true;
	}


	//Consola debug.
	VkDebugUtilsMessengerEXT DebugConsole;


	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	//Extensiones de la gpu.
	const std::vector<const char*> gpuExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	//Vértices del triángulo.
	const std::vector<Vertex> Vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> Indices = {
		0, 1, 2, 2, 3, 0
	};

};

int main() {

	VulkanGame game;
	
	try {
		game.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;

		return -1;
	}

	return 0;
}