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

#include "AssetManager.h"
#include "Texture.h"
#include "Model3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "EntityComponentSystem.h"
#include "Window.h"

#include "RendererExceptions.h"

#include <GLFW/glfw3.h>
#include <set>
#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS; 

const static DynamicArray<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const static DynamicArray<uint32_t> ignoredValidationLayersMessages = {
	0x609a13b, // Shader attachmentt not used

	0xd6d77e1e, // Dynamic Rendering Color
	0x151f5e5a, // Dynamic Rendering Depth
	0x11b37e31, 
	0x6c16bfb4
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

	Engine::GetLogger()->Log(level, std::string(pCallbackData->pMessage) + "\n");

	return VK_FALSE;
}


RendererVk::RendererVk(bool requestRayTracing) : IRenderer(RenderApiType::VULKAN, requestRayTracing) {
	m_implicitResizeHandling = true;
}

RendererVk::~RendererVk() {
	// Close();
}

void RendererVk::Initialize(const std::string& appName, const Version& version, const IO::IDisplay& display, PresentMode mode) {	
	CreateInstance(appName, version);

	if (AreValidationLayersAvailable())
		SetupDebugLogging();
	CreateSurface(display);
	ChooseGpu();
	
	SetupDebugFunctions(GetGpu()->As<GpuVk>()->GetLogicalDevice());

	CreateCommandQueues();
	CreateSyncPrimitives();
	CreateSwapchain(mode, display.GetResolution());
	CreateGpuMemoryAllocator();
	if (IsRtRequested() && GetGpu()->As<GpuVk>()->GetInfo().IsRtCompatible())
		SetupRtFunctions(GetGpu()->As<GpuVk>()->GetLogicalDevice());
	SetupRenderingFunctions(GetGpu()->As<GpuVk>()->GetLogicalDevice());

	CreateMainRenderpass();

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->CreateTargetImage(display.GetResolution());
}

OwnedPtr<ICommandPool> RendererVk::CreateCommandPool(const ICommandQueue* targetQueueType) {
	return new CommandPoolVk(
		*GetGpu()->As<GpuVk>(),
		targetQueueType->GetFamily(),
		targetQueueType->GetQueueType());
}

OwnedPtr<IGraphicsPipeline> RendererVk::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) {
	auto* pipeline = new GraphicsPipelineVk;
	pipeline->Create(&layout, GetGpu(), pipelineInfo, vertexInfo);

	return pipeline;
}

OwnedPtr<IRaytracingPipeline> RendererVk::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) {
	auto* pipeline = new RaytracingPipelineVk();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}

OwnedPtr<IComputePipeline> RendererVk::_CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	auto* pipeline = new ComputePipelineVk();
	pipeline->Create(layout, pipelineInfo);

	return pipeline;
}

void RendererVk::WaitForCompletion() {
	const VkDevice device = GetGpu()->As<GpuVk>()->GetLogicalDevice();

	// Esperar a que termine la ejecución de todos los comandos.
	vkDeviceWaitIdle(device);
}

void RendererVk::Close() {
	const VkDevice device = GetGpu()->As<GpuVk>()->GetLogicalDevice();
	WaitForCompletion();

	CloseSingletonInstances();
	

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	for (USize32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		vkDestroyFence(device, m_fullyRenderedFences[i], nullptr);

		vkDestroySemaphore(device, m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, m_imageFinishedSemaphores[i], nullptr);
	}

	CloseGpu();

	if (AreValidationLayersAvailable()) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		// OSK_ASSERT(func != nullptr, "No se puede destruir la consola de capas de validación.");
		if (func) {
			func(m_instance, m_debugConsole, nullptr);
		}
	}

	vkDestroyInstance(m_instance, nullptr);
}

void RendererVk::HandleResize(const Vector2ui& resolution) {
	// El renderpass final se maneja automáticamente en el bucle principal del renderer.
	IRenderer::HandleResize(resolution);
}

void RendererVk::SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) {
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

	m_singleTimeCommandLists.Insert(commandList.GetPointer());
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

	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)version.mayor, (int)version.menor, (int)version.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION(Engine::GetVersion().mayor, Engine::GetVersion().menor, Engine::GetVersion().parche);
	appInfo.apiVersion = vulkanVersion;

	Engine::GetLogger()->InfoLog(std::format("Versión de vulkan: {}.{}.{}",
		VK_VERSION_MAJOR(vulkanVersion),
		VK_VERSION_MINOR(vulkanVersion),
		VK_VERSION_PATCH(vulkanVersion)));

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

	//Engine::GetLogger()->InfoLog("Extensiones del renderizador: ");
	//for (const auto& i : extensions)
	//	Engine::GetLogger()->InfoLog("	" + std::string(i));

	//Engine::GetLogger()->InfoLog("Capas de validación del renderizador: ");
	//for (const auto& i : validationLayers)
	//	Engine::GetLogger()->InfoLog("	" + std::string(i));

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("Crear instancia de Vulkan", result));
}

void RendererVk::CreateSwapchain(PresentMode mode, const Vector2ui& resolution) {
	DynamicArray<UIndex32> queueIndices{};

	if (UseUnifiedCommandQueue()) {
		queueIndices.Insert(GetUnifiedQueue()->GetFamily().familyIndex);
	}
	else {
		queueIndices.Insert(GetGraphicsComputeQueue()->GetFamily().familyIndex);
		queueIndices.Insert(GetPresentationQueue()->GetFamily().familyIndex);
	}

	_SetSwapchain(new SwapchainVk(
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

void RendererVk::CreateSurface(const IO::IDisplay& display) {
	const VkResult result = glfwCreateWindowSurface(m_instance, display.As<IO::Window>()->_GetGlfw(), nullptr, &m_surface);
	OSK_ASSERT(result == VK_SUCCESS, RendererCreationException("No se ha podido crear la superficie", result));
}

void RendererVk::ChooseGpu() {
	// --------------- Physical ----------------- //

	// Obtiene el número de GPUs disponibles.
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(m_instance, &count, nullptr);

	OSK_ASSERT(count != 0, GpuNotFoundException());

	// Obtiene los handlers de las GPUs.
	auto devices = DynamicArray<VkPhysicalDevice>::CreateResizedArray(count);
	vkEnumeratePhysicalDevices(m_instance, &count, devices.GetData());

	// Comprobar la compatibilidad de las GPUs.
	// Obtener una GPU compatible.
	DynamicArray<GpuVk::Info> gpus;
	for (const VkPhysicalDevice gpu : devices) {
		const GpuVk::Info info = GpuVk::Info::Get(gpu, m_surface);

		if (info.isSuitable)
			gpus.Insert(info);
	}

	OSK_ASSERT(!gpus.IsEmpty(), GpuNotFoundException());

	VkPhysicalDevice gpu = devices[0];
	GpuVk::Info info = gpus[0];

	Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(info.properties.deviceName));

	auto* gpuVk = new GpuVk(gpu, m_surface);
	gpuVk->CreateLogicalDevice();

	_SetGpu(gpuVk);
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

		_SetUnifiedCommandQueue(new CommandQueueVk(unifiedFamiliesList[0], 0, GpuQueueType::MAIN,  gpu));

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

		_SetGraphicsCommputeCommandQueue(new CommandQueueVk(graphicsAndComputeFamily, 0, GpuQueueType::MAIN, gpu));

		RegisterGraphicsCommputeCommandPool(GetGraphicsComputeQueue());

		// Una para presentación.
		const QueueFamily presentationFamily = queueFamilies.GetFamilies(
			CommandsSupport::PRESENTATION)[0];

		_SetPresentationCommandQueue(new CommandQueueVk(presentationFamily, 0, GpuQueueType::PRESENTATION, gpu));

		_SetMainCommandList(GetGraphicsComputeCommandPool(std::this_thread::get_id())->CreateCommandList(gpu));

		OSK::Engine::GetLogger()->InfoLog("Uso de colas GPU de renderizado y presentación.");
	}

	// Buscamos una cola exclusivamente de transferencia.
	const DynamicArray<QueueFamily> transferFamilies = queueFamilies.GetFamilies(
		CommandsSupport::TRANSFER);

	for (const auto& family : transferFamilies) {
		if (family.support == CommandsSupport::TRANSFER) {
			_SetTransferOnlyCommandQueue(new CommandQueueVk(family, 0, GpuQueueType::ASYNC_TRANSFER, gpu));
			RegisterTransferOnlyCommandPool(GetTransferOnlyQueue());
			OSK::Engine::GetLogger()->InfoLog("Uso de cola GPU de transferencia.");

			break;
		}
	}
}

bool RendererVk::AreValidationLayersAvailable() const {
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

OwnedPtr<IMaterialSlot> RendererVk::_CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const {
	return new MaterialSlotVk(name, &layout);
}

void RendererVk::CreateSyncPrimitives() {
	const VkDevice logicalDevice = GetGpu()->As<GpuVk>()->GetLogicalDevice();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		
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
	const VkFence firstFencesToReset[] = {
		m_fullyRenderedFences[0]
	};
	vkResetFences(logicalDevice, _countof(firstFencesToReset), firstFencesToReset);
}

void RendererVk::CreateGpuMemoryAllocator() {
	_SetMemoryAllocator(new GpuMemoryAllocatorVk(GetGpu()));
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
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_imageAvailableSemaphores[currentCommandBufferIndex]; // Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.pSignalSemaphores = &m_imageFinishedSemaphores[currentCommandBufferIndex]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &GetMainCommandList()->As<CommandListVk>()->GetCommandBuffers()[currentCommandBufferIndex];

	const VkQueue queue = UseUnifiedCommandQueue()
		? GetUnifiedQueue()->As<CommandQueueVk>()->GetQueue()
		: GetGraphicsComputeQueue()->As<CommandQueueVk>()->GetQueue();

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, m_fullyRenderedFences[currentCommandBufferIndex]);
	OSK_ASSERT(result == VK_SUCCESS, CommandListSubmitException("PreCompute", result));
}

void RendererVk::SubmitFrame() {
	const VkDevice logicalDevice = GetGpu()->As<GpuVk>()->GetLogicalDevice();

	const VkSwapchainKHR swapChains = _GetSwapchain()->As<SwapchainVk>()->GetSwapchain();
	// Esperamos a que la imagen haya terminado de rendereizarse.
	const VkSemaphore waitSemaphores = m_imageFinishedSemaphores[currentCommandBufferIndex];

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &waitSemaphores; // Debemos esperar a que la imagen actual termine de renderizarse.
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChains;
	presentInfo.pImageIndices = &currentFrameIndex; // Lo presentamos en esta imagen.
	presentInfo.pResults = nullptr;

	const VkQueue queue = UseUnifiedCommandQueue()
		? GetUnifiedQueue()->As<CommandQueueVk>()->GetQueue()
		: GetPresentationQueue()->As<CommandQueueVk>()->GetQueue();

	VkResult result = vkQueuePresentKHR(queue, &presentInfo);
	const bool resized = result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
	if (resized) {
		// Esperamos a que se terminen todas las listas de comandos para
		// poder cambiar de tamaño los render targets.
		vkDeviceWaitIdle(logicalDevice);
		/// @todo vkWaitForSemaphores

		const auto& resolution = Engine::GetDisplay()->GetResolution();

		if (resolution.x > 0 && resolution.y > 0) {
			_GetSwapchain()->As<SwapchainVk>()->Resize(*GetGpu(), resolution);
			Engine::GetRenderer()->As<RendererVk>()->HandleResize(resolution);
		}
	}

	currentCommandBufferIndex = (currentCommandBufferIndex + 1) % NUM_RESOURCES_IN_FLIGHT;

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

PFN_vkGetBufferDeviceAddressKHR RendererVk::pvkGetBufferDeviceAddressKHR = nullptr;
PFN_vkCmdBuildAccelerationStructuresKHR RendererVk::pvkCmdBuildAccelerationStructuresKHR = nullptr;
PFN_vkBuildAccelerationStructuresKHR RendererVk::pvkBuildAccelerationStructuresKHR = nullptr;
PFN_vkCreateAccelerationStructureKHR RendererVk::pvkCreateAccelerationStructureKHR = nullptr;
PFN_vkDestroyAccelerationStructureKHR RendererVk::pvkDestroyAccelerationStructureKHR = nullptr;
PFN_vkGetAccelerationStructureBuildSizesKHR RendererVk::pvkGetAccelerationStructureBuildSizesKHR = nullptr;
PFN_vkGetAccelerationStructureDeviceAddressKHR RendererVk::pvkGetAccelerationStructureDeviceAddressKHR = nullptr;
PFN_vkCmdTraceRaysKHR RendererVk::pvkCmdTraceRaysKHR = nullptr;
PFN_vkGetRayTracingShaderGroupHandlesKHR RendererVk::pvkGetRayTracingShaderGroupHandlesKHR = nullptr;
PFN_vkCreateRayTracingPipelinesKHR RendererVk::pvkCreateRayTracingPipelinesKHR = nullptr;

PFN_vkSetDebugUtilsObjectNameEXT RendererVk::pvkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkSetDebugUtilsObjectTagEXT RendererVk::pvkSetDebugUtilsObjectTagEXT = nullptr;
PFN_vkCmdDebugMarkerBeginEXT RendererVk::pvkCmdDebugMarkerBeginEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT RendererVk::pvkCmdInsertDebugUtilsLabelEXT = nullptr;
PFN_vkCmdBeginDebugUtilsLabelEXT RendererVk::pvkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT RendererVk::pvkCmdEndDebugUtilsLabelEXT = nullptr;

PFN_vkCmdBeginRendering RendererVk::pvkCmdBeginRendering = nullptr;
PFN_vkCmdEndRendering RendererVk::pvkCmdEndRendering = nullptr;
