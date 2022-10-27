#include <vulkan/vulkan.h>

#include "RendererVulkan.h"

#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"
#include "GpuVulkan.h"
#include "CommandQueueVulkan.h"
#include "CommandListVulkan.h"
#include "CommandPoolVulkan.h"
#include "Version.h"
#include "Window.h"
#include "FormatVulkan.h"
#include "Format.h"
#include "SwapchainVulkan.h"
#include "Version.h"
#include "SyncDeviceVulkan.h"
#include "DynamicArray.hpp"
#include "GpuMemoryAllocatorVulkan.h"
#include "RenderpassType.h"
#include "Color.hpp"
#include "GraphicsPipelineVulkan.h"
#include "PipelineCreateInfo.h"
#include "IGpuVertexBuffer.h"
#include "Vertex.h"
#include "Viewport.h"
#include "GpuIndexBufferVulkan.h"
#include "RenderApiType.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialSlotVulkan.h"
#include "IGpuUniformBuffer.h"
#include "RaytracingPipelineVulkan.h"
#include "ComputePipelineVulkan.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"

#include "AssetManager.h"
#include "Texture.h"
#include "Model3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "EntityComponentSystem.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <set>
#include <ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS; 

const DynamicArray<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const DynamicArray<uint32_t> ignoredValidationLayersMessages = {
	0x609a13b, // Shader attachmentt not used

	0xd6d77e1e, // Dynamic Rendering
	0x6c16bfb4
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	// Message severity:
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

	// Tipos de mensaje:
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

	if (ignoredValidationLayersMessages.ContainsElement(pCallbackData->messageIdNumber))
		return 0;

	IO::LogLevel level = IO::LogLevel::WARNING;

	switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return 0;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		level = IO::LogLevel::WARNING;
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		level = IO::LogLevel::L_ERROR;
		break;

	default:
		level = IO::LogLevel::WARNING;
		break;
	}

	Engine::GetLogger()->Log(level, std::string(pCallbackData->pMessage));

	return VK_FALSE;
}


RendererVulkan::RendererVulkan(bool requestRayTracing) : IRenderer(RenderApiType::VULKAN, requestRayTracing) {
	implicitResizeHandling = true;
}

RendererVulkan::~RendererVulkan() {
	Close();
}

void RendererVulkan::Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) {
	this->window = &window;
	
	CreateInstance(appName, version);

	if (AreValidationLayersAvailable())
		SetupDebugLogging();
	CreateSurface(window);
	ChooseGpu();
	if (AreValidationLayersAvailable())
		SetupDebugFunctions(currentGpu->As<GpuVulkan>()->GetLogicalDevice());

	CreateCommandQueues();

	CreateSwapchain(mode);
	CreateSyncDevice();
	CreateGpuMemoryAllocator();
	if (IsRtRequested() && currentGpu->As<GpuVulkan>()->GetInfo().IsRtCompatible())
		SetupRtFunctions(currentGpu->As<GpuVulkan>()->GetLogicalDevice());
	SetupRenderingFunctions(currentGpu->As<GpuVulkan>()->GetLogicalDevice());

	renderTargetsCamera = new ECS::CameraComponent2D;
	renderTargetsCamera->LinkToWindow(&window);
	renderTargetsCameraTransform.SetScale({ window.GetWindowSize().X / 2.0f, window.GetWindowSize().Y / 2.0f});
	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	CreateMainRenderpass();

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->CreateTargetImage(window.GetWindowSize());

	isOpen = true;
}

OwnedPtr<IGraphicsPipeline> RendererVulkan::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) {
	GraphicsPipelineVulkan* pipeline = new GraphicsPipelineVulkan;
	pipeline->Create(&layout, currentGpu.GetPointer(), pipelineInfo, vertexInfo);

	return pipeline;
}

OwnedPtr<IRaytracingPipeline> RendererVulkan::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) {
	RaytracingPipelineVulkan* pipeline = new RaytracingPipelineVulkan();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}

OwnedPtr<IComputePipeline> RendererVulkan::_CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	ComputePipelineVulkan* pipeline = new ComputePipelineVulkan();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}

void RendererVulkan::Close() {
	syncDevice.Delete();
	swapchain.Delete();

	finalRenderTarget.Delete();

	renderTargetsCamera.Delete();

	graphicsCommandPool.Delete();
	computeCommandPool.Delete();

	materialSystem.Delete();
	gpuMemoryAllocator.Delete();

	vkDestroySurfaceKHR(instance, surface, nullptr);

	currentGpu.Delete();

	if (AreValidationLayersAvailable()) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		OSK_ASSERT(func != nullptr, "No se puede destruir la consola de capas de validación.");
		func(instance, debugConsole, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

	isOpen = false;
}

void RendererVulkan::HandleResize() {
	// El renderpass final se maneja automáticamente en el bucle principal del renderer.
	IRenderer::HandleResize();
}

void RendererVulkan::SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) {
	const TSize cmdIndex = commandList->GetCommandListIndex();
	const VkQueue graphicsQ = graphicsQueue->As<CommandQueueVulkan>()->GetQueue();
	const VkCommandBuffer cmdBuffer = commandList->As<CommandListVulkan>()->GetCommandBuffers()->At(cmdIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	VkResult result = vkQueueSubmit(graphicsQ, 1, &submitInfo, VK_NULL_HANDLE);
	OSK_ASSERT(result == VK_SUCCESS, "Error al enviar comando de uso único. Code: " + std::to_string(result));
	result = vkQueueWaitIdle(graphicsQ);
	OSK_ASSERT(result == VK_SUCCESS, "Error al esperar comando de uso único. Code: " + std::to_string(result));

	vkFreeCommandBuffers(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), graphicsCommandPool->As<CommandPoolVulkan>()->GetCommandPool(), 1, &cmdBuffer);

	singleTimeCommandLists.Insert(commandList.GetPointer());
}

void RendererVulkan::CreateInstance(const std::string& appName, const Version& version) {
	// Obtenemos la versión de vulkan soportada
	PFN_vkEnumerateInstanceVersion pvkEnumeratInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
	if (pvkEnumeratInstanceVersion == nullptr) {
		vulkanVersion = VK_API_VERSION_1_0;
	}
	else {
		pvkEnumeratInstanceVersion(&vulkanVersion);
	}

	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)version.mayor, (int)version.menor, (int)version.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION(Engine::GetVersion().mayor, Engine::GetVersion().menor, Engine::GetVersion().parche);
	appInfo.apiVersion = vulkanVersion;

	Engine::GetLogger()->InfoLog("Versión de vulkan: "
		+ std::to_string(VK_VERSION_MAJOR(vulkanVersion)) + "."
		+ std::to_string(VK_VERSION_MINOR(vulkanVersion)) + "."
		+ std::to_string(VK_VERSION_PATCH(vulkanVersion)));

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
	auto extensions = DynamicArray<const char*>::CreateReservedArray(glfwExtensionCount);
	for (size_t i = 0; i < glfwExtensionCount; i++)
		extensions.Insert(glfwExtensions[i]);

#ifdef OSK_DEBUG
	extensions.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//Capas de validación.

	if (AreValidationLayersAvailable()) {
		Engine::GetLogger()->DebugLog("Capas de validación activas.");

		createInfo.enabledLayerCount = validationLayers.GetSize();
		createInfo.ppEnabledLayerNames = validationLayers.GetData();
	}
	else {
		Engine::GetLogger()->Log(IO::LogLevel::WARNING, "No se ha encontrado soporte para las capas de validación.");
	}
#endif

	createInfo.enabledExtensionCount = extensions.GetSize();
	createInfo.ppEnabledExtensionNames = extensions.GetData();
	createInfo.pNext = nullptr;

	//Engine::GetLogger()->InfoLog("Extensiones del renderizador: ");
	//for (const auto& i : extensions)
	//	Engine::GetLogger()->InfoLog("	" + std::string(i));

	//Engine::GetLogger()->InfoLog("Capas de validación del renderizador: ");
	//for (const auto& i : validationLayers)
	//	Engine::GetLogger()->InfoLog("	" + std::string(i));

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	OSK_ASSERT(result == VK_SUCCESS, "Crear instancia de Vulkan." + std::to_string(result));
}

void RendererVulkan::CreateSwapchain(PresentMode mode) {
	swapchain = new SwapchainVulkan;

	swapchain->As<SwapchainVulkan>()->Create(mode, Format::B8G8R8A8_SRGB, *currentGpu->As<GpuVulkan>(), *window);
	Engine::GetLogger()->InfoLog("Creado el swapchain.");
}

void RendererVulkan::SetupDebugLogging() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	OSK_ASSERT(func != nullptr, "No se puede iniciar la consola de capas de validación.");

	auto result = func(instance, &createInfo, nullptr, &debugConsole);
	OSK_ASSERT(result == VK_SUCCESS, "No se puede iniciar la consola de capas de validación.");

	Engine::GetLogger()->InfoLog("Capas de validación activas.");
}

void RendererVulkan::CreateSurface(const IO::Window& window) {
	const VkResult result = glfwCreateWindowSurface(instance, window._GetGlfw(), nullptr, &surface);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear la superficie. " + std::to_string(result));
}

void RendererVulkan::ChooseGpu() {
	currentGpu = new GpuVulkan;

	// --------------- Physical ----------------- //

	//Obtiene el número de GPUs disponibles.
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);

	OSK_ASSERT(count != 0, "No hay ninguna GPU compatible con Vulkan.");

	//Obtiene las GPUs.
	auto devices = DynamicArray<VkPhysicalDevice>::CreateResizedArray(count);
	vkEnumeratePhysicalDevices(instance, &count, devices.GetData());

	//Comprobar la compatibilidad de las GPUs.
	//Obtener una GPU compatible.
	DynamicArray<GpuVulkan::Info> gpus;
	for (const auto& gpu : devices) {
		auto info = GpuVulkan::Info::Get(gpu, surface);

		if (info.isSuitable)
				gpus.Insert(info);
	}

	OSK_ASSERT(!gpus.IsEmpty(), "No hay una GPU compatible.");

	VkPhysicalDevice gpu = devices[0];
	GpuVulkan::Info info = gpus[0];
	vkGetPhysicalDeviceMemoryProperties(gpu, &info.memoryProperties);

	OSK_ASSERT(gpu != VK_NULL_HANDLE, "No hay ninguna GPU compatible con Vulkan.");

	Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(info.properties.deviceName));

	currentGpu->As<GpuVulkan>()->SetPhysicalDevice(gpu);
	currentGpu->As<GpuVulkan>()->SetInfo(info);

	currentGpu->As<GpuVulkan>()->CreateLogicalDevice(surface);
}

void RendererVulkan::CreateCommandQueues() {
	graphicsQueue = new CommandQueueVulkan;
	presentQueue = new CommandQueueVulkan;
	computeQueue = new CommandQueueVulkan;

	const QueueFamilyIndices queueFamilyIndices = currentGpu->As<GpuVulkan>()->GetQueueFamilyIndices(surface);

	// Obtener las colas.
	graphicsQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.graphicsFamily.value(), *currentGpu->As<GpuVulkan>());
	presentQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.presentFamily.value(), *currentGpu->As<GpuVulkan>());
	computeQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.computeFamily.value(), *currentGpu->As<GpuVulkan>());

	if (graphicsQueue->As<CommandQueueVulkan>()->GetQueueIndex() == computeQueue->As<CommandQueueVulkan>()->GetQueueIndex())
		singleCommandQueue = true;

	graphicsCommandPool = currentGpu->As<GpuVulkan>()->CreateGraphicsCommandPool().GetPointer();
	computeCommandPool = currentGpu->As<GpuVulkan>()->CreateComputeCommandPool().GetPointer();

	graphicsCommandPool->As<CommandPoolVulkan>()->SetSwapchainCount(3);
	computeCommandPool->As<CommandPoolVulkan>()->SetSwapchainCount(3);

	graphicsCommandList = graphicsCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
	graphicsCommandList->SetDebugName("Graphics CmdList");

	if (!singleCommandQueue) {
		preComputeCommandList = computeCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
		postComputeCommandList = computeCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
		frameBuildCommandList = graphicsCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();

		preComputeCommandList->SetDebugName("Pre-Compute CmdList");
		postComputeCommandList->SetDebugName("Post-Compute CmdList");
		frameBuildCommandList->SetDebugName("Frame Build CmdList");
	}
}

bool RendererVulkan::AreValidationLayersAvailable() const {
#ifdef OSK_DEBUG
	// Obtenemos el número de capas.
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Obtenemos las capas.
	auto availableLayers = DynamicArray<VkLayerProperties>::CreateResizedArray(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

	//Capas de validación necesitadas.
	for (auto layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;

				break;
			}
		}

		if (layerFound)
			return true;
	}

	return false;
#else
	return false;
#endif 
}

OwnedPtr<IMaterialSlot> RendererVulkan::_CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const {
	return new MaterialSlotVulkan(name, &layout);
}

void RendererVulkan::CreateSyncDevice() {
	const VkDevice logicalDevice = currentGpu->As<GpuVulkan>()->GetLogicalDevice();

	imageAvailableSemaphores = DynamicArray<VkSemaphore>::CreateResizedArray(3, VK_NULL_HANDLE);
	renderFinishedSemaphores = DynamicArray<VkSemaphore>::CreateResizedArray(3, VK_NULL_HANDLE);
	preComputeFinishedSemaphores = DynamicArray<VkSemaphore>::CreateResizedArray(3, VK_NULL_HANDLE);
	postComputeFinishedSemaphores = DynamicArray<VkSemaphore>::CreateResizedArray(3, VK_NULL_HANDLE);
	frameBuildSemaphores = DynamicArray<VkSemaphore>::CreateResizedArray(3, VK_NULL_HANDLE);

	fullyRenderedFences = DynamicArray<VkFence>::CreateResizedArray(3, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (TSize i = 0; i < 3; i++) {
		
		VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el semáforo.");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &preComputeFinishedSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el semáforo.");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el semáforo.");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &postComputeFinishedSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el semáforo.");

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &frameBuildSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el semáforo.");

		// Fences
		result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fullyRenderedFences[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear el fence.");

		if (pvkSetDebugUtilsObjectNameEXT != nullptr) {
			std::string name = "";

			VkDebugUtilsObjectNameInfoEXT debugName{};
			debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			debugName.objectType = VK_OBJECT_TYPE_SEMAPHORE;

			name = "Image Available Semaphore[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)imageAvailableSemaphores[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			name = "Render Finished Semaphore[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)renderFinishedSemaphores[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			name = "Pre-Compute Finished Semaphore[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)preComputeFinishedSemaphores[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			name = "Post-Compute Finished Semaphore[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)postComputeFinishedSemaphores[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			name = "Frame build Semaphore[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)frameBuildSemaphores[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);


			debugName.objectType = VK_OBJECT_TYPE_FENCE;

			name = "Fully Rendered Fence[" + std::to_string(i) + "]";
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)fullyRenderedFences[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);
		}

	}

	// Las primeras fencees en enviarse deben estár UNSIGNALED,
	// pero las hemos creado signaled.
	const VkFence firstFencesToReset[] = {
		fullyRenderedFences[0]
	};
	vkResetFences(logicalDevice, _countof(firstFencesToReset), firstFencesToReset);
}

void RendererVulkan::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorVulkan(currentGpu.GetPointer());
}

void RendererVulkan::PresentFrame() {
	if (isFirstRender) {
		AcquireNextFrame();

		graphicsCommandList->Reset();
		graphicsCommandList->Start();

		if (!singleCommandQueue) {
			preComputeCommandList->Reset();
			preComputeCommandList->Start();

			postComputeCommandList->Reset();
			postComputeCommandList->Start();

			frameBuildCommandList->Reset();
			frameBuildCommandList->Start();
		}

		isFirstRender = false;
	}

	graphicsCommandList->Close();
	if (!singleCommandQueue) {
		preComputeCommandList->Close();
		postComputeCommandList->Close();
		frameBuildCommandList->Close();
	}

	// Sync
	if (singleCommandQueue) {
		SubmitGraphicsAndComputeCommands();
	}
	else {
		SubmitPreComputeCommands();
		SubmitGraphicsCommands();
		SubmitPostComputeCommands();
		SubmitFrameBuildCommands();
	}
	SubmitFrame();
	AcquireNextFrame();

	for (TSize i = 0; i < singleTimeCommandLists.GetSize(); i++)
		singleTimeCommandLists.At(i)->DeleteAllStagingBuffers();
	singleTimeCommandLists.Free();

	//

	graphicsCommandList->Reset();
	graphicsCommandList->Start();

	if (!singleCommandQueue) {
		preComputeCommandList->Reset();
		preComputeCommandList->Start();

		postComputeCommandList->Reset();
		postComputeCommandList->Start();

		frameBuildCommandList->Reset();
		frameBuildCommandList->Start();
	}
}

void RendererVulkan::SubmitPreComputeCommands() {
	// Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentCommandBufferIndex]; // Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &preComputeFinishedSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &preComputeCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(currentCommandBufferIndex);

	VkResult result = vkQueueSubmit(computeQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE /*preComputeCommandsFences[currentCommandBufferIndex]*/);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola PreCompute.");
}

void RendererVulkan::SubmitGraphicsCommands() {
	// Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
	const VkSemaphore waitSemaphores[] = {
		preComputeFinishedSemaphores[currentCommandBufferIndex]
	};

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = _countof(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &graphicsCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(currentCommandBufferIndex);

	VkResult result = vkQueueSubmit(graphicsQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE /*graphicsCommandsFences[currentCommandBufferIndex]*/);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola gráfica.");
}

void RendererVulkan::SubmitPostComputeCommands() {
	// Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	const VkSemaphore waitSemaphores[] = {
		renderFinishedSemaphores[currentCommandBufferIndex]
	};

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = _countof(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &postComputeFinishedSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &postComputeCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(currentCommandBufferIndex);

	VkResult result = vkQueueSubmit(computeQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola PostCompute.");
}

void RendererVulkan::SubmitGraphicsAndComputeCommands() {
	// Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentCommandBufferIndex]; // Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &frameBuildSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &graphicsCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(currentCommandBufferIndex);

	VkResult result = vkQueueSubmit(graphicsQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, fullyRenderedFences[currentCommandBufferIndex]);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola PreCompute.");
}

void RendererVulkan::SubmitFrameBuildCommands() {
	// Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
	const VkSemaphore waitSemaphores[] = {
		postComputeFinishedSemaphores[currentCommandBufferIndex]
	};

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = _countof(waitSemaphores);
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &frameBuildSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &frameBuildCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(currentCommandBufferIndex);

	VkResult result = vkQueueSubmit(graphicsQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, fullyRenderedFences[currentCommandBufferIndex]);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola FrameBuild.");
}

void RendererVulkan::SubmitFrame() {
	const VkSwapchainKHR swapChains[] = { swapchain->As<SwapchainVulkan>()->GetSwapchain() };
	const VkSemaphore waitSemaphores[] = { frameBuildSemaphores[currentCommandBufferIndex] };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = _countof(waitSemaphores);
	presentInfo.pWaitSemaphores = waitSemaphores; // Debemos esperar a que la imagen actual termine de renderizarse.
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &currentFrameIndex; // Lo presentamos en esta imagen.
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(presentQueue->As<CommandQueueVulkan>()->GetQueue(), &presentInfo);
	const bool resized = result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
	if (resized) {
		// Esperamos a que se terminen todas las listas de comandos para
		// poder cambiar de tamaño los render targets.
		vkDeviceWaitIdle(currentGpu->As<GpuVulkan>()->GetLogicalDevice());
		/// @todo vkWaitForSemaphores

		swapchain->As<SwapchainVulkan>()->Resize();

		if (window->GetWindowSize().X > 0 && window->GetWindowSize().Y > 0)
			Engine::GetRenderer()->As<RendererVulkan>()->HandleResize();
	}

	currentCommandBufferIndex = (currentCommandBufferIndex + 1) % NUM_RESOURCES_IN_FLIGHT; //swapchain->GetImageCount()

	// Si la siguiente imagen está siendo procesada, esperar a que termine.
	// @todo vkGetSemaphoreState
	vkWaitForFences(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), 1, &fullyRenderedFences[currentCommandBufferIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), 1, &fullyRenderedFences[currentCommandBufferIndex]);
}

void RendererVulkan::AcquireNextFrame() {
	// Adquirimos el índice de la próxima imagen a procesar.
	// NOTA: puede que tengamos que esperar a que esta imagen quede disponible.
	VkResult result = vkAcquireNextImageKHR(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), swapchain->As<SwapchainVulkan>()->GetSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentCommandBufferIndex], VK_NULL_HANDLE, &currentFrameIndex);
	OSK_CHECK(result == VK_SUCCESS, "vkAcquireNextImageKHR code: " + std::to_string(result));
}

void RendererVulkan::SetupRtFunctions(VkDevice device) {
	pvkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));
	pvkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
	pvkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR"));
	pvkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
	pvkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
	pvkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
	pvkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
	pvkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
	pvkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
	pvkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));

	isRtActive = true;
}

void RendererVulkan::SetupDebugFunctions(VkDevice instance) {
	pvkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
	pvkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetDeviceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT"));
	pvkCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(instance, "vkCmdDebugMarkerBeginEXT"));
	pvkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetDeviceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
	pvkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
}

void RendererVulkan::SetupRenderingFunctions(VkDevice logicalDevice) {
	if (VK_VERSION_MAJOR(vulkanVersion) >= 1 && VK_VERSION_MINOR(vulkanVersion) >= 3) {
		pvkCmdBeginRendering = vkCmdBeginRendering;
		pvkCmdEndRendering = vkCmdEndRendering;

		if (pvkCmdBeginRendering == nullptr or pvkCmdEndRendering == nullptr) {
			pvkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRendering"));
			pvkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRendering"));
		}
	}
	else {
		pvkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRendering"));
		pvkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRendering"));
	}

	OSK_ASSERT(pvkCmdBeginRendering != nullptr, "No se puede iniciar el renderizador, falta Dynamic Rendering.");
	OSK_ASSERT(pvkCmdEndRendering != nullptr, "No se puede iniciar el renderizador, falta Dynamic Rendering.");
}

TSize RendererVulkan::GetCurrentFrameIndex() const {
	return currentFrameIndex;
}

TSize RendererVulkan::GetCurrentCommandListIndex() const {
	return currentCommandBufferIndex;
}

bool RendererVulkan::SupportsRaytracing() const {
	return currentGpu->As<GpuVulkan>()->GetInfo().IsRtCompatible();
}

PFN_vkGetBufferDeviceAddressKHR RendererVulkan::pvkGetBufferDeviceAddressKHR = nullptr;
PFN_vkCmdBuildAccelerationStructuresKHR RendererVulkan::pvkCmdBuildAccelerationStructuresKHR = nullptr;
PFN_vkBuildAccelerationStructuresKHR RendererVulkan::pvkBuildAccelerationStructuresKHR = nullptr;
PFN_vkCreateAccelerationStructureKHR RendererVulkan::pvkCreateAccelerationStructureKHR = nullptr;
PFN_vkDestroyAccelerationStructureKHR RendererVulkan::pvkDestroyAccelerationStructureKHR = nullptr;
PFN_vkGetAccelerationStructureBuildSizesKHR RendererVulkan::pvkGetAccelerationStructureBuildSizesKHR = nullptr;
PFN_vkGetAccelerationStructureDeviceAddressKHR RendererVulkan::pvkGetAccelerationStructureDeviceAddressKHR = nullptr;
PFN_vkCmdTraceRaysKHR RendererVulkan::pvkCmdTraceRaysKHR = nullptr;
PFN_vkGetRayTracingShaderGroupHandlesKHR RendererVulkan::pvkGetRayTracingShaderGroupHandlesKHR = nullptr;
PFN_vkCreateRayTracingPipelinesKHR RendererVulkan::pvkCreateRayTracingPipelinesKHR = nullptr;

PFN_vkSetDebugUtilsObjectNameEXT RendererVulkan::pvkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkSetDebugUtilsObjectTagEXT RendererVulkan::pvkSetDebugUtilsObjectTagEXT = nullptr;
PFN_vkCmdDebugMarkerBeginEXT RendererVulkan::pvkCmdDebugMarkerBeginEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT RendererVulkan::pvkCmdInsertDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT RendererVulkan::pvkCmdEndDebugUtilsLabelEXT = nullptr;

PFN_vkCmdBeginRendering RendererVulkan::pvkCmdBeginRendering = nullptr;
PFN_vkCmdEndRendering RendererVulkan::pvkCmdEndRendering = nullptr;
