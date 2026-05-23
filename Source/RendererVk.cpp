#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#ifdef OSK_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#include "AndroidDisplay.h"
#endif

#include <vulkan/vulkan.h>

#include "RendererVk.h"

#include "OSKengine.h"
#include "Logger.h"
#include "Assert.h"
#include "GpuVk.h"
#include "CommandQueueVk.h"
#include "CommandListVk.h"
#include "CommandPoolVk.h"
#include "Version.h"
#include "Window.h"
#include "FormatVk.h"
#include "Format.h"
#include "SwapchainVk.h"
#include "Version.h"
#include "DynamicArray.hpp"
#include "GpuMemoryAllocatorVk.h"
#include "RenderpassType.h"
#include "Color.hpp"
#include "GraphicsPipelineVk.h"
#include "PipelineCreateInfo.h"
#include "GpuBuffer.h"
#include "Vertex.h"
#include "Viewport.h"
#include "RenderApiType.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialSlotVk.h"
#include "RaytracingPipelineVk.h"
#include "ComputePipelineVk.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "MeshPipelineVk.h"

#include "AssetManager.h"
#include "Texture.h"
#include "Model3D.h"
#include "TransformComponent3D.h"
#include "ModelComponent3D.h"
#include "EntityComponentSystem.h"
#include "Window.h"

#include "RendererExceptions.h"

#ifdef OSK_PC_ANY
#include <GLFW/glfw3.h>
#endif
#ifdef OSK_ANDROID
#include "AndroidDisplay.h"
#endif

#include <set>
#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS; 

const static DynamicArray<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const static DynamicArray<uint32_t> ignoredValidationLayersMessages = {
	// 0x609a13b, // Shader attachmentt not used

	// 0xd6d77e1e, // Dynamic Rendering Color
	// 0x151f5e5a, // Dynamic Rendering Depth
	// 0x11b37e31, 
	// 0x6c16bfb4
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
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

	Engine::GetLogger()->Log(level, std::to_string(pCallbackData->messageIdNumber), "  ", std::string(pCallbackData->pMessage), "\n");

	if (pCallbackData->messageIdNumber == -303265468) {
		Engine::GetLogger()->DebugLog("XD");
	}

	return VK_FALSE;
}


RendererVk::RendererVk(bool requestRayTracing) : IRenderer(RenderApiType::VULKAN, requestRayTracing) {
	m_implicitResizeHandling = true;
}

void RendererVk::Initialize(const std::string& appName, const Version& version, IO::IDisplay& display, PresentMode mode) {	
	m_presentMode = mode;

	CreateInstance(appName, version);

	if (AreValidationLayersAvailable())
		SetupDebugLogging();
	CreateSurface(display);
	ChooseGpu();
	
	auto* gpu = GetGpu()->As<GpuVk>();

	SetupDebugFunctions(gpu->GetLogicalDevice());

	CreateCommandQueues();
	CreateSyncPrimitives();
	CreateSwapchain(mode, display.GetResolution());
	CreateGpuMemoryAllocator();

	if (IsRtRequested() && gpu->GetInfo().IsRtCompatible()) {
		SetupRtFunctions(gpu->GetLogicalDevice());
	}

	if (gpu->GetInfo().IsCompatibleWithMeshShaders()) {
		SetupMeshFunctions(gpu->GetLogicalDevice());
	}

	if (gpu->GetInfo().SupportsDynamicRendering()) {
		SetupRenderingFunctions(gpu->GetLogicalDevice());
	}

	CreateMainRenderpass();

	if (Engine::GetEcs()) {
		for (auto i : Engine::GetEcs()->GetRenderSystems()) {
			i->CreateTargetImage(display.GetResolution());
		}
	}
}

void RendererVk::ResetDisplay(IO::IDisplay& display) {
	CreateSurface(display);
	GetGpu()->As<GpuVk>()->SetSurface(m_surface);
	CreateSwapchain(m_presentMode, display.GetResolution());
	CreateMainRenderpass();
}

UniquePtr<ICommandPool> RendererVk::CreateCommandPool(const ICommandQueue* targetQueueType) {
	return MakeUnique<CommandPoolVk>(
		*GetGpu()->As<GpuVk>(),
		targetQueueType->As<CommandQueueVk>()->GetFamily(),
		targetQueueType->GetQueueType());
}

UniquePtr<IGraphicsPipeline> RendererVk::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) {
	auto pipeline = MakeUnique<GraphicsPipelineVk>();
	pipeline->Create(&layout, GetGpu(), pipelineInfo, vertexInfo);

	return pipeline;
}

UniquePtr<IMeshPipeline> RendererVk::_CreateMeshPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	auto pipeline = MakeUnique<MeshPipelineVk>();
	pipeline->Create(&layout, GetGpu(), pipelineInfo);

	return pipeline;
}

UniquePtr<IRaytracingPipeline> RendererVk::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) {
	auto pipeline = MakeUnique<RaytracingPipelineVk>();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}


UniquePtr<IComputePipeline> RendererVk::_CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	auto pipeline = MakeUnique<ComputePipelineVk>();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}


void RendererVk::WaitForCompletion() {
	const VkDevice device = GetGpu()->As<GpuVk>()->GetLogicalDevice();

	// Esperar a que termine la ejecución de todos los comandos.
	vkDeviceWaitIdle(device);

#ifdef OSK_ANDROID
	CreateSyncPrimitives();
#endif // OSK_ANDROID


	// Reseteamos las imágenes adquiridas.
	m_isFirstRender = true;
}


void RendererVk::Close() {
	const VkDevice device = GetGpu()->As<GpuVk>()->GetLogicalDevice();
	WaitForCompletion();

	CloseSingletonInstances();
	

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	for (USize32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		vkDestroyFence(device, m_fullyRenderedFences[i], nullptr);

		vkDestroySemaphore(device, m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, m_imageFinishedSemaphores[i], nullptr);
	}

	// Esto elimina los renderpasses, que
	// deben eliminarse antes que la GPU.
	GetMainCommandList()->ClearImagesCache();
	CloseGpu();

	if (AreValidationLayersAvailable()) {
		if (auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT")) {
			func(m_instance, m_debugConsole, nullptr);
		}
	}

	vkDestroyInstance(m_instance, nullptr);
}


void RendererVk::SubmitSingleUseCommandList(UniquePtr<ICommandList>&& commandList) {
	std::lock_guard lock(m_queueSubmitMutex.mutex);

	const auto cmdIndex = commandList->_GetCommandListIndex();

	// Debemos saber cual es la cola compatible con el pool usado.
	VkQueue queue = VK_NULL_HANDLE;

	if (UseUnifiedCommandQueue() && commandList->GetOwnerPool()->GetLinkedQueueType() == GetUnifiedQueue()->GetQueueType()) {
		// Cola unificada.
		queue = GetUnifiedQueue()->As<CommandQueueVk>()->GetQueue();
		// Engine::GetLogger()->InfoLog(std::format("\tInsertada lista en cola unificada."));
	}
	else if (!UseUnifiedCommandQueue() && commandList->GetOwnerPool()->GetLinkedQueueType() == GetGraphicsComputeQueue()->GetQueueType()) {
		// Cola principal.
		queue = GetGraphicsComputeQueue()->As<CommandQueueVk>()->GetQueue();
		// Engine::GetLogger()->InfoLog(std::format("\tInsertada lista en cola principal."));
	}
	else if (HasTransferOnlyCommandPool() && commandList->GetOwnerPool()->GetLinkedQueueType() == GetTransferOnlyQueue()->GetQueueType()) {
		// Cola de transferencia.
		queue = GetTransferOnlyQueue()->As<CommandQueueVk>()->GetQueue();
		// Engine::GetLogger()->InfoLog(std::format("\tInsertada lista en cola exclusiva de transferencia."));
	}

	const VkCommandBuffer cmdBuffer = commandList->As<CommandListVk>()->GetCommandBuffers()[cmdIndex];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	OSK_ASSERT(result == VK_SUCCESS, CommandQueueSubmitException(result));
	result = vkQueueWaitIdle(queue);
	OSK_ASSERT(result == VK_SUCCESS, CommandQueueSubmitException(result));

	vkFreeCommandBuffers(
		GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		commandList->GetOwnerPool()->As<CommandPoolVk>()->GetCommandPool(),
		1, &cmdBuffer);

	m_singleTimeCommandLists.Insert(std::move(commandList));
}


void RendererVk::CreateInstance(const std::string& appName, const Version& version) {
	// Obtenemos la versión de vulkan soportada
	auto pvkEnumeratInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
	if (pvkEnumeratInstanceVersion == nullptr) {
		vulkanVersion = VK_API_VERSION_1_0;
	}
	else {
		pvkEnumeratInstanceVersion(&vulkanVersion);
	}

#ifdef OSK_ANDROID
	vulkanVersion = VK_API_VERSION_1_1;
#endif // OSK_ANDROID


	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)version.mayor, (int)version.menor, (int)version.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION(Engine::GetVersion().mayor, Engine::GetVersion().menor, Engine::GetVersion().parche);
	appInfo.apiVersion = vulkanVersion;

	Engine::GetLogger()->InfoLog(std::format("Versión de vulkan: {}.{}.{}",
		VK_API_VERSION_MAJOR(vulkanVersion),
		VK_API_VERSION_MINOR(vulkanVersion),
		VK_API_VERSION_PATCH(vulkanVersion)));

	//Create info.
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Establecemos las extensiones.
	//Extensiones de la ventana.
#ifdef OSK_PC_ANY
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Extensiones totales.
	auto extensions = DynamicArray<const char*>::CreateReserved(glfwExtensionCount);
	for (size_t i = 0; i < glfwExtensionCount; i++)
		extensions.Insert(glfwExtensions[i]);
#endif
#ifdef OSK_ANDROID
	auto extensions = DynamicArray<const char*>::CreateReserved(2);
	extensions.Insert("VK_KHR_surface");
	extensions.Insert("VK_KHR_android_surface");
#endif
	
	extensions.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	extensions.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#ifdef OSK_DEBUG
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

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.GetSize());
	createInfo.ppEnabledExtensionNames = extensions.GetData();
	createInfo.pNext = nullptr;

	Engine::GetLogger()->InfoLog("Extensiones del renderizador: ");
	for (const auto& i : extensions)
		Engine::GetLogger()->InfoLog("	" + std::string(i));

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	Engine::GetLogger()->DebugLog(std::format("Resultado de la inicialización: {}", (int)result));
	OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("Crear instancia de Vulkan", result));
	
	Engine::GetLogger()->DebugLog("Finalizada inicialización del renderizador.");
}


void RendererVk::CreateSwapchain(PresentMode mode, const Vector2ui& resolution) {
	DynamicArray<UIndex32> queueIndices{};

	if (UseUnifiedCommandQueue()) {
		queueIndices.Insert(GetUnifiedQueue()->As<CommandQueueVk>()->GetFamily().familyIndex);
	}
	else {
		queueIndices.Insert(GetGraphicsComputeQueue()->As<CommandQueueVk>()->GetFamily().familyIndex);
		queueIndices.Insert(GetPresentationQueue()->As<CommandQueueVk>()->GetFamily().familyIndex);
	}

	_SetSwapchain(MakeUnique<SwapchainVk>(
		mode,
		Format::BGRA8_SRGB,
		*GetGpu()->As<GpuVk>(),
		resolution,
		queueIndices.GetFullSpan()));
}


void RendererVk::SetupDebugLogging() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	OSK_ASSERT(func != nullptr, RendererCreationException("No se puede iniciar la consola de capas de validación", 0));

	auto result = func(m_instance, &createInfo, nullptr, &m_debugConsole);
	OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("No se puede iniciar la consola de capas de validación.", 0));

	Engine::GetLogger()->InfoLog("Capas de validación activas.");
}


void RendererVk::CreateSurface(IO::IDisplay& display) {
	if (m_surface) {
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}

#ifdef OSK_WINDOWS
	const VkResult result = glfwCreateWindowSurface(m_instance, display.As<IO::Window>()->_GetGlfw(), nullptr, &m_surface);
#endif // OSK_WINDOWS

#ifdef OSK_ANDROID
	VkAndroidSurfaceCreateInfoKHR info{};
	info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	info.pNext = nullptr;
	info.window = display.As<IO::AndroidWindow>()->GetNativeWindow();
	info.flags = 0;
	const VkResult result = vkCreateAndroidSurfaceKHR(m_instance, &info, nullptr, &m_surface);
#endif // OSK_ANDROID


	OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("No se ha podido crear la superficie", result));
}


void RendererVk::ChooseGpu() {
	// --------------- Physical ----------------- //

	// Obtiene el número de GPUs disponibles.
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(m_instance, &count, nullptr);

	OSK_ASSERT(count != 0, GpuNotFoundException());

	// Obtiene los handlers de las GPUs.
	auto devices = DynamicArray<VkPhysicalDevice>::CreateResized(count);
	vkEnumeratePhysicalDevices(m_instance, &count, devices.GetData());

	// Comprobar la compatibilidad de las GPUs.
	// Obtener una GPU compatible.
	DynamicArray<GpuInfoVkAny> gpus;
	for (const VkPhysicalDevice gpu : devices) {
		const auto info = GpuInfoVkAny::Get(gpu, m_surface);

		if (info.isSuitable) {
			gpus.Insert(info);
		}
	}

	OSK_ASSERT(!gpus.IsEmpty(), GpuNotFoundException());

	VkPhysicalDevice gpu = devices[0];

	GpuInfoVkAny info = gpus[0];

	bool hasDiscrete = false;
	for (UIndex64 i = 0; i < devices.GetSize(); i++) {
		if (gpus[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			gpu = devices[i];
			info = gpus[i];

			hasDiscrete = true;
		}
	}

	Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(info.properties.deviceName));

	auto gpuVk = MakeUnique<GpuVk>(gpu, m_surface);
	gpuVk->CreateLogicalDevice();

	_SetGpu(std::move(gpuVk));
}


void RendererVk::CreateCommandQueues() {
	GpuVk& gpu = *GetGpu()->As<GpuVk>();

	const QueueFamiles queueFamilies = gpu.GetQueueFamilyIndices();

	// Obtener las colas.
	
	// Preferir cola única para comandos y gráficos.
	// Según el spec, si hay al menos una familia de comandos gráficos, también debe soportar computación y transfer.
	
	const DynamicArray<QueueFamily> unifiedFamiliesList = queueFamilies.GetFamilies(
		CommandsSupport::GRAPHICS |
		CommandsSupport::COMPUTE |
		CommandsSupport::TRANSFER |
		CommandsSupport::PRESENTATION);

	if (!unifiedFamiliesList.IsEmpty()) {
		// Existe una familia con soporte para todos los comandos:
		// usar cola unificada.

		_SetUnifiedCommandQueue(MakeUnique<CommandQueueVk>(unifiedFamiliesList[0], 0, GpuQueueType::MAIN,  gpu));

		RegisterUnifiedCommandPool(GetUnifiedQueue());

		_SetMainCommandList(GetUnifiedCommandPool(std::this_thread::get_id())->CreateCommandList(gpu));

		OSK::Engine::GetLogger()->InfoLog("Uso de cola GPU unificada.");
	}
	else {
		// Colas potencialmente exclusivas para cada tarea.
		// 
		// Una para graphics + compute + transfer (debe existir según spec.).
		const QueueFamily graphicsAndComputeFamily = queueFamilies.GetFamilies(
			CommandsSupport::GRAPHICS |
			CommandsSupport::COMPUTE |
			CommandsSupport::TRANSFER)[0];

		_SetGraphicsCommputeCommandQueue(MakeUnique<CommandQueueVk>(graphicsAndComputeFamily, 0, GpuQueueType::MAIN, gpu));

		RegisterGraphicsCommputeCommandPool(GetGraphicsComputeQueue());

		// Una para presentación.
		const QueueFamily presentationFamily = queueFamilies.GetFamilies(
			CommandsSupport::PRESENTATION)[0];

		_SetPresentationCommandQueue(MakeUnique<CommandQueueVk>(presentationFamily, 0, GpuQueueType::PRESENTATION, gpu));

		_SetMainCommandList(GetGraphicsComputeCommandPool(std::this_thread::get_id())->CreateCommandList(gpu));

		OSK::Engine::GetLogger()->InfoLog("Uso de colas GPU de renderizado y presentación.");
	}

	// Buscamos una cola exclusivamente de transferencia.
	const DynamicArray<QueueFamily> transferFamilies = queueFamilies.GetFamilies(
		CommandsSupport::TRANSFER);

	for (const auto& family : transferFamilies) {
		if (family.support == CommandsSupport::TRANSFER) {
			_SetTransferOnlyCommandQueue(MakeUnique<CommandQueueVk>(family, 0, GpuQueueType::ASYNC_TRANSFER, gpu));
			RegisterTransferOnlyCommandPool(GetTransferOnlyQueue());
			OSK::Engine::GetLogger()->InfoLog("Uso de cola GPU de transferencia.");

			break;
		}
	}
}


bool RendererVk::AreValidationLayersAvailable() const {
#ifdef OSK_ANDROID // TODO: quitar
	// Obtenemos el número de capas.
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Obtenemos las capas.
	auto availableLayers = DynamicArray<VkLayerProperties>::CreateResized(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Número de capas de validación: ", layerCount);
	for (const auto& layer : availableLayers) {
		Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Capa de validación comprobada: ", layer.layerName);
	}

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

#ifdef OSK_DEBUG
	// Obtenemos el número de capas.
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Obtenemos las capas.
	auto availableLayers = DynamicArray<VkLayerProperties>::CreateResized(layerCount);
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

#endif
}


UniquePtr<IMaterialSlot> RendererVk::_CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const {
	return MakeUnique<MaterialSlotVk>(name, &layout);
}


void RendererVk::CreateSyncPrimitives() {
	const VkDevice logicalDevice = GetGpu()->As<GpuVk>()->GetLogicalDevice();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		
		VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("Error al crear el semáforo.", result));

		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_imageFinishedSemaphores[i]);
		OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("Error al crear el semáforo.", result));


		// Fences
		result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &m_fullyRenderedFences[i]);
		OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("Error al crear el fence.", result));


		if (pvkSetDebugUtilsObjectNameEXT != nullptr) {
			std::string name = "";

			VkDebugUtilsObjectNameInfoEXT debugName{};
			debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			debugName.objectType = VK_OBJECT_TYPE_SEMAPHORE;

			name = std::format("Image Available Semaphore[{}]", i);
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)m_imageAvailableSemaphores[i];
			pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			name = std::format("Render Finished Semaphore[{}]", i);
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)m_imageFinishedSemaphores[i];
			pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);

			debugName.objectType = VK_OBJECT_TYPE_FENCE;

			name = std::format("Fully Rendered Fence[{}]", i);
			debugName.pObjectName = name.c_str();
			debugName.objectHandle = (uint64_t)m_fullyRenderedFences[i];
			result = pvkSetDebugUtilsObjectNameEXT(logicalDevice, &debugName);
		}

	}

	// Las primeras fencees en enviarse deben estár UNSIGNALED,
	// pero las hemos creado signaled.
	vkResetFences(logicalDevice, 1, &m_fullyRenderedFences[0]);
}


void RendererVk::CreateGpuMemoryAllocator() {
	_SetMemoryAllocator(MakeUnique<GpuMemoryAllocatorVk>(GetGpu()));
}


void RendererVk::PresentFrame() {
	if (m_isFirstRender) {
		AcquireNextFrame();

		GetMainCommandList()->Reset();
		GetMainCommandList()->Start();

		m_isFirstRender = false;
	}

	GetMainCommandList()->Close();

	// Sync
	SubmitMainCommandList();
	SubmitFrame();
	AcquireNextFrame();

	{
		std::lock_guard lock(m_queueSubmitMutex.mutex);
		for (auto& singleTimeCmdList : m_singleTimeCommandLists)
			singleTimeCmdList->DeleteAllStagingBuffers();
		m_singleTimeCommandLists.Free();
	}

	GetAllocator()->FreeStagingMemory();

	//

	GetMainCommandList()->Reset();
	GetMainCommandList()->Start();
}


void RendererVk::SubmitMainCommandList() {
	// Esperar a que se completen todos los comandos.
	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_imageAvailableSemaphores[currentCommandBufferIndex];
	submitInfo.pWaitDstStageMask = &waitStage;

	// Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.pSignalSemaphores = &m_imageFinishedSemaphores[currentCommandBufferIndex]; 
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &GetMainCommandList()->As<CommandListVk>()->GetCommandBuffers()[currentCommandBufferIndex];

	const VkQueue queue = UseUnifiedCommandQueue()
		? GetUnifiedQueue()->As<CommandQueueVk>()->GetQueue()
		: GetGraphicsComputeQueue()->As<CommandQueueVk>()->GetQueue();

	// Indicamos el fence que será avisado cuando terminde de procesarse el command buffer.
	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, m_fullyRenderedFences[currentCommandBufferIndex]);
	OSK_ASSERT(result == VK_SUCCESS, CommandListSubmitException("PreCompute", result));
}


void RendererVk::SubmitFrame() {
	const auto logicalDevice = GetGpu()->As<GpuVk>()->GetLogicalDevice();
	const auto swapChains = _GetSwapchain()->As<SwapchainVk>()->GetSwapchain();

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	// Debemos esperar a que la imagen actual termine de renderizarse.
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_imageFinishedSemaphores[currentCommandBufferIndex];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChains;
	presentInfo.pImageIndices = &currentFrameIndex; // Lo presentamos en esta imagen.
	presentInfo.pResults = nullptr;

	const VkQueue queue = UseUnifiedCommandQueue()
		? GetUnifiedQueue()->As<CommandQueueVk>()->GetQueue()
		: GetPresentationQueue()->As<CommandQueueVk>()->GetQueue();

	VkResult result = vkQueuePresentKHR(queue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		// Esperamos a que se terminen todas las listas de comandos para
		// poder cambiar de tamaño los render targets.
		vkDeviceWaitIdle(logicalDevice);
		/// @todo vkWaitForSemaphores

		const auto& resolution = Engine::GetDisplay()->GetResolution();

		if (resolution.x > 0 && resolution.y > 0) {
			_GetSwapchain()->As<SwapchainVk>()->Resize(*GetGpu(), resolution);
			Engine::GetRenderer()->As<RendererVk>()->HandleResize(resolution);
		}

		GetMainCommandList()->ClearImagesCache();
	}

	currentCommandBufferIndex = (currentCommandBufferIndex + 1) % GetSwapchainImagesCount();

	// Si la siguiente imagen está siendo procesada, esperar a que termine.
	vkWaitForFences(logicalDevice, 1, &m_fullyRenderedFences[currentCommandBufferIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice, 1, &m_fullyRenderedFences[currentCommandBufferIndex]);
}


void RendererVk::AcquireNextFrame() {
	const static uint64_t OSK_NO_TIMEOUT = UINT64_MAX;

	// Adquirimos el índice de la próxima imagen a procesar.
	// NOTA: puede que tengamos que esperar a que esta imagen quede disponible.
	VkResult result = vkAcquireNextImageKHR(
		GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		_GetSwapchain()->As<SwapchainVk>()->GetSwapchain(),
		OSK_NO_TIMEOUT,
		m_imageAvailableSemaphores[currentCommandBufferIndex], 
		VK_NULL_HANDLE, 
		&currentFrameIndex);

	// OSK_CHECK(result == VK_SUCCESS, "vkAcquireNextImageKHR code: " + std::to_string(result));
}


void RendererVk::SetupRtFunctions(VkDevice device) {
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

	m_isRtActive = true;
}


void RendererVk::SetupMeshFunctions(VkDevice logicalDevice) {
	pvkCmdDrawMeshTasksEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksEXT"));
}


void RendererVk::SetupDebugFunctions(VkDevice instance) {
	pvkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
	pvkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetDeviceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT"));
	pvkCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(instance, "vkCmdDebugMarkerBeginEXT"));
	pvkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetDeviceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
	pvkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
	pvkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
}


void RendererVk::SetupRenderingFunctions(VkDevice logicalDevice) {
	if (VK_VERSION_MAJOR(vulkanVersion) >= 1 && VK_VERSION_MINOR(vulkanVersion) >= 3) {
		pvkCmdBeginRendering = vkCmdBeginRendering;
		pvkCmdEndRendering = vkCmdEndRendering;

		if (pvkCmdBeginRendering == nullptr || pvkCmdEndRendering == nullptr) {
			pvkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRendering"));
			pvkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRendering"));
		}
	}
	else {
		pvkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRendering"));
		pvkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRendering"));
	}

	OSK_ASSERT(pvkCmdBeginRendering != nullptr, GpuNotCompatibleException("No se puede iniciar el renderizador, falta Dynamic Rendering."));
	OSK_ASSERT(pvkCmdEndRendering != nullptr, GpuNotCompatibleException("No se puede iniciar el renderizador, falta Dynamic Rendering."));
}


UIndex32 RendererVk::GetCurrentFrameIndex() const {
	return currentFrameIndex;
}


UIndex32 RendererVk::GetCurrentCommandListIndex() const {
	return currentCommandBufferIndex;
}


bool RendererVk::SupportsRaytracing() const {
	return GetGpu()->As<GpuVk>()->GetInfo().IsRtCompatible();
}

#define OSK_VK_DEF_FUNC(funcName) \
PFN_##funcName RendererVk::p##funcName = nullptr;

// Ray-tracing
OSK_VK_DEF_FUNC(vkGetBufferDeviceAddressKHR);
OSK_VK_DEF_FUNC(vkCmdBuildAccelerationStructuresKHR);
OSK_VK_DEF_FUNC(vkBuildAccelerationStructuresKHR);
OSK_VK_DEF_FUNC(vkCreateAccelerationStructureKHR);
OSK_VK_DEF_FUNC(vkDestroyAccelerationStructureKHR);
OSK_VK_DEF_FUNC(vkGetAccelerationStructureBuildSizesKHR);
OSK_VK_DEF_FUNC(vkGetAccelerationStructureDeviceAddressKHR);
OSK_VK_DEF_FUNC(vkCmdTraceRaysKHR);
OSK_VK_DEF_FUNC(vkGetRayTracingShaderGroupHandlesKHR);
OSK_VK_DEF_FUNC(vkCreateRayTracingPipelinesKHR);

// Debug
OSK_VK_DEF_FUNC(vkSetDebugUtilsObjectNameEXT);
OSK_VK_DEF_FUNC(vkSetDebugUtilsObjectTagEXT);
OSK_VK_DEF_FUNC(vkCmdDebugMarkerBeginEXT);
OSK_VK_DEF_FUNC(vkCmdInsertDebugUtilsLabelEXT);
OSK_VK_DEF_FUNC(vkCmdBeginDebugUtilsLabelEXT);
OSK_VK_DEF_FUNC(vkCmdEndDebugUtilsLabelEXT);

// Dynamic rendering
OSK_VK_DEF_FUNC(vkCmdBeginRendering);
OSK_VK_DEF_FUNC(vkCmdEndRendering);

// Mesh shaders
OSK_VK_DEF_FUNC(vkCmdDrawMeshTasksEXT);

#endif
