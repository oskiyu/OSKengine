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
	0x609a13b // Shader attachmentt not used
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
		level = IO::LogLevel::INFO;
		return 0;
		pCallbackData->pMessage;
		break;

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
	CreateCommandQueues();
	CreateSwapchain(mode);
	CreateSyncDevice();
	CreateGpuMemoryAllocator();
	//SetupRtFunctions(currentGpu->As<GpuVulkan>()->GetLogicalDevice());

	renderTargetsCamera = new ECS::CameraComponent2D;
	renderTargetsCamera->LinkToWindow(&window);
	renderTargetsCameraTransform.SetScale({ window.GetWindowSize().X / 2.0f, window.GetWindowSize().Y / 2.0f});
	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	CreateMainRenderpass();

	if (Engine::GetEntityComponentSystem())
		for (auto i : Engine::GetEntityComponentSystem()->GetRenderSystems())
			i->CreateTargetImage(window.GetWindowSize());

	isOpen = true;
}

OwnedPtr<IGraphicsPipeline> RendererVulkan::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexInfo) {
	GraphicsPipelineVulkan* pipeline = new GraphicsPipelineVulkan;
	pipeline->Create(layout, currentGpu.GetPointer(), pipelineInfo, vertexInfo);

	return pipeline;
}

OwnedPtr<IRaytracingPipeline> RendererVulkan::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexTypeName) {
	RaytracingPipelineVulkan* pipeline = new RaytracingPipelineVulkan();
	pipeline->Create(*layout, pipelineInfo);

	return pipeline;
}

void RendererVulkan::Close() {
	Engine::GetLogger()->InfoLog("Destruyendo el renderizador de Vulkan.");

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

void RendererVulkan::SubmitSingleUseCommandList(ICommandList* commandList) {
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

	singleTimeCommandLists.Insert(commandList);
}

void RendererVulkan::CreateInstance(const std::string& appName, const Version& version) {
	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)version.mayor, (int)version.menor, (int)version.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION(Engine::GetVersion().mayor, Engine::GetVersion().menor, Engine::GetVersion().parche);
	appInfo.apiVersion = VK_API_VERSION_1_3;

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

	Engine::GetLogger()->InfoLog("Extensiones del renderizador: ");
	for (const auto& i : extensions)
		Engine::GetLogger()->InfoLog("	" + std::string(i));

	Engine::GetLogger()->InfoLog("Capas de validación del renderizador: ");
	for (const auto& i : validationLayers)
		Engine::GetLogger()->InfoLog("	" + std::string(i));

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

	Engine::GetLogger()->InfoLog("Se ha creado correctamente la superficie de Vulkan.");
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

		gpus.Insert(info);
	}

	VkPhysicalDevice gpu = devices[0];
	GpuVulkan::Info info = gpus[0];
	vkGetPhysicalDeviceMemoryProperties(gpu, &info.memoryProperties);

	OSK_ASSERT(gpu != VK_NULL_HANDLE, "No hay ninguna GPU compatible con Vulkan.");

	Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(info.properties.deviceName));

	currentGpu->As<GpuVulkan>()->SetPhysicalDevice(gpu);
	currentGpu->As<GpuVulkan>()->SetInfo(info);
}

void RendererVulkan::CreateCommandQueues() {
	graphicsQueue = new CommandQueueVulkan;
	presentQueue = new CommandQueueVulkan;

	currentGpu->As<GpuVulkan>()->CreateLogicalDevice(surface);
	const QueueFamilyIndices queueFamilyIndices = currentGpu->As<GpuVulkan>()->GetQueueFamilyIndices(surface);

	// Obtener las colas.
	graphicsQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.graphicsFamily.value(), *currentGpu->As<GpuVulkan>());
	presentQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.presentFamily.value(), *currentGpu->As<GpuVulkan>());
	computeQueue->As<CommandQueueVulkan>()->Create(queueFamilyIndices.computeFamily.value(), *currentGpu->As<GpuVulkan>());

	graphicsCommandPool = currentGpu->As<GpuVulkan>()->CreateGraphicsCommandPool().GetPointer();
	computeCommandPool = currentGpu->As<GpuVulkan>()->CreateComputeCommandPool().GetPointer();

	graphicsCommandPool->As<CommandPoolVulkan>()->SetSwapchainCount(3);
	computeCommandPool->As<CommandPoolVulkan>()->SetSwapchainCount(3);

	graphicsCommandList = graphicsCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
	computeCommandList = computeCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
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

OwnedPtr<IMaterialSlot> RendererVulkan::_CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const {
	return new MaterialSlotVulkan(name, layout);
}

void RendererVulkan::CreateSyncDevice() {
	syncDevice = currentGpu->As<GpuVulkan>()->CreateSyncDevice().GetPointer();

	syncDevice->As<SyncDeviceVulkan>()->SetDevice(*currentGpu->As<GpuVulkan>());
	syncDevice->As<SyncDeviceVulkan>()->SetSwapchain(*swapchain->As<SwapchainVulkan>());
}

void RendererVulkan::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorVulkan(currentGpu.GetPointer());
}

void RendererVulkan::PresentFrame() {
	if (isFirstRender) {
		const bool shouldResize = syncDevice->As<SyncDeviceVulkan>()->UpdateCurrentFrameIndex();
		if (shouldResize)
			return;

		graphicsCommandList->Reset();
		graphicsCommandList->Start();

		computeCommandList->Reset();
		computeCommandList->Start();

		isFirstRender = false;
	}
	graphicsCommandList->Close();
	computeCommandList->Close();

	syncDevice->As<SyncDeviceVulkan>()->Flush(*graphicsQueue->As<CommandQueueVulkan>(), *presentQueue->As<CommandQueueVulkan>(), *graphicsCommandList->As<CommandListVulkan>());

	// Sync
	const bool shouldResize = syncDevice->As<SyncDeviceVulkan>()->UpdateCurrentFrameIndex();
	//if (shouldResize)
		//return;

	for (TSize i = 0; i < singleTimeCommandLists.GetSize(); i++)
		singleTimeCommandLists.At(i)->DeleteAllStagingBuffers();

	//

	graphicsCommandList->Reset();
	graphicsCommandList->Start();

	computeCommandList->Reset();
	computeCommandList->Start();
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

TSize RendererVulkan::GetCurrentFrameIndex() const {
	return syncDevice->As<SyncDeviceVulkan>()->GetCurrentFrameIndex();
}

TSize RendererVulkan::GetCurrentCommandListIndex() const {
	return syncDevice->As<SyncDeviceVulkan>()->GetCurrentCommandListIndex();
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