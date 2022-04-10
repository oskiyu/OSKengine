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
#include "RenderpassVulkan.h"
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

#include "AssetManager.h"
#include "Texture.h"
#include "Model3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "EntityComponentSystem.h"

#include <GLFW/glfw3.h>
#include <set>
#include <ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS; 

const DynamicArray<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	//Message severity:
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: "diagnostic" message.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: información.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning.
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error.

	//Tipos de mensaje:
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: algo ha ocurrido, no tiene que ver con la especificación o el rendimiento.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: algo ha ocurrido, incumple la especificación.
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: algo ha ocurrido, uso no óptimo de vulkan.

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


RendererVulkan::RendererVulkan() : IRenderer(RenderApiType::VULKAN) {

}

RendererVulkan::~RendererVulkan() {
	Close();
}

void RendererVulkan::Initialize(const std::string& appName, const Version& version, const IO::Window& window) {
	this->window = &window;
	
	CreateInstance(appName, version);

	if (AreValidationLayersAvailable())
		SetupDebugLogging();

	CreateSurface(window);
	ChooseGpu();
	CreateCommandQueues();
	CreateSwapchain();
	CreateSyncDevice();
	CreateGpuMemoryAllocator();
	CreateMainRenderpass();

	isOpen = true;
}

OwnedPtr<IGraphicsPipeline> RendererVulkan::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass, const VertexInfo vertexInfo) {
	GraphicsPipelineVulkan* pipeline = new GraphicsPipelineVulkan(renderpass->As<RenderpassVulkan>());

	pipeline->Create(layout, currentGpu.GetPointer(), pipelineInfo, vertexInfo);

	return pipeline;
}

void RendererVulkan::Close() {
	Engine::GetLogger()->InfoLog("Destruyendo el renderizador de Vulkan.");

	syncDevice.Delete();
	swapchain.Delete();
	renderpass.Delete();
	commandPool.Delete();
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
	// Se maneja automáticamente en el bucle principal del renderer.
}

void RendererVulkan::SubmitSingleUseCommandList(ICommandList* commandList) {
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandList->As<CommandListVulkan>()->GetCommandBuffers()->At(0);

	vkQueueSubmit(graphicsQueue->As<CommandQueueVulkan>()->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue->As<CommandQueueVulkan>()->GetQueue());

	vkFreeCommandBuffers(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), 
		commandPool->As<CommandPoolVulkan>()->GetCommandPool(), 1, &commandList->As<CommandListVulkan>()->GetCommandBuffers()->At(0));

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

void RendererVulkan::CreateSwapchain() {
	swapchain = new SwapchainVulkan;

	swapchain->As<SwapchainVulkan>()->Create(Format::B8G8R8A8_SRGB, *currentGpu->As<GpuVulkan>(), *window);
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
	auto result = glfwCreateWindowSurface(instance, window._GetGlfw(), nullptr, &surface);
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

	VkQueue graphicsQ;
	VkQueue presentQ;

	currentGpu->As<GpuVulkan>()->CreateLogicalDevice(surface);
	auto queueFamilyIndices = currentGpu->As<GpuVulkan>()->GetQueueFamilyIndices(surface);

	// Obtener las colas.
	vkGetDeviceQueue(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQ);
	vkGetDeviceQueue(currentGpu->As<GpuVulkan>()->GetLogicalDevice(), queueFamilyIndices.presentFamily.value(), 0, &presentQ);

	graphicsQueue->As<CommandQueueVulkan>()->SetQueue(graphicsQ);
	presentQueue->As<CommandQueueVulkan>()->SetQueue(presentQ);

	commandPool = currentGpu->As<GpuVulkan>()->CreateCommandPool().GetPointer();
	Engine::GetLogger()->InfoLog("Creada el pool de comandos.");

	commandPool->As<CommandPoolVulkan>()->SetSwapchainCount(3);
	commandList = commandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
	Engine::GetLogger()->InfoLog("Creada la lista de comandos.");
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

OwnedPtr<IRenderpass> RendererVulkan::CreateSecondaryRenderpass(GpuImage* targetImage0, GpuImage* targetImage1, GpuImage* targetImage2) {
	OwnedPtr<IRenderpass> output = new RenderpassVulkan(RenderpassType::INTERMEDIATE);
	output->As<RenderpassVulkan>()->Create(nullptr, targetImage0->GetFormat());

	targetImage1 ? output->SetImages(targetImage0, targetImage1, targetImage2) : output->SetImages(targetImage0, targetImage0, targetImage0);

	materialSystem->RegisterRenderpass(output.GetPointer());

	return output;
}

void RendererVulkan::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorVulkan(currentGpu.GetPointer());

	Engine::GetLogger()->InfoLog("Creado el asignador de memoria de la GPU.");
}

void RendererVulkan::CreateMainRenderpass() {
	renderpass = new RenderpassVulkan(RenderpassType::FINAL);
	renderpass->As<RenderpassVulkan>()->CreateFinalPresent(swapchain.GetPointer());
	renderpass->SetImages(swapchain->GetImage(0), swapchain->GetImage(1), swapchain->GetImage(2));

	materialSystem->RegisterRenderpass(renderpass.GetPointer());
}

void RendererVulkan::PresentFrame() {
	if (isFirstRender) {
		commandList->Start();
		commandList->BeginAndClearRenderpass(renderpass.GetPointer(), Color::RED());
		Vector4ui windowRec = {
			0,
			0,
			window->GetWindowSize().X,
			window->GetWindowSize().Y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		commandList->SetViewport(viewport);
		commandList->SetScissor(windowRec);

		isFirstRender = false;

		return;
	}

	commandList->EndRenderpass(renderpass.GetPointer());
	commandList->Close();

	for (TSize i = 0; i < singleTimeCommandLists.GetSize(); i++)
		singleTimeCommandLists.At(i)->DeleteAllStagingBuffers();

	syncDevice->As<SyncDeviceVulkan>()->FirstAwait();
	auto result = syncDevice->As<SyncDeviceVulkan>()->UpdateCurrentFrameIndex();
	if (result)
		return;
	syncDevice->As<SyncDeviceVulkan>()->Flush(*graphicsQueue->As<CommandQueueVulkan>() , *presentQueue->As<CommandQueueVulkan>(), *commandList->As<CommandListVulkan>());
	
	//
	
	commandList->Reset();
	commandList->Start();

	commandList->BeginAndClearRenderpass(renderpass.GetPointer(), Color::BLACK());

	Vector4ui windowRec = {
		0,
		0,
		window->GetWindowSize().X,
		window->GetWindowSize().Y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);
}
