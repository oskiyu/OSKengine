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

#include <GLFW/glfw3.h>
#include <set>
#include <vector>

using namespace OSK;

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

	LogLevel level = LogLevel::WARNING;

	switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		level = LogLevel::INFO;
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		level = LogLevel::WARNING;
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		level = LogLevel::L_ERROR;
		break;

	default:
		level = LogLevel::WARNING;
		break;
	}

	Engine::GetLogger()->Log(level, std::string(pCallbackData->pMessage) + "\n");

	return VK_FALSE;
}


RendererVulkan::~RendererVulkan() {
	Close();
}

void RendererVulkan::Initialize(const std::string& appName, const Version& version, const Window& window) {
	this->window = &window;
	
	CreateInstance(appName, version);

	if (AreValidationLayersAvailable())
		SetupDebugLogging();

	CreateSurface(window);
	ChooseGpu();
	CreateCommandQueues();
	CreateSwapchain();
	CreateSyncDevice();
}

void RendererVulkan::Close() {
	Engine::GetLogger()->InfoLog("Destruyendo el renderizador de Vulkan.");

	currentGpu->Close();

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void RendererVulkan::CreateInstance(const std::string& appName, const Version& version) {
	//Información de la app.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION((int)version.mayor, (int)version.menor, (int)version.parche);
	appInfo.pEngineName = "OSKengine";
	appInfo.engineVersion = VK_MAKE_VERSION((int)0, (int)0, (int)0);
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
	//Capas de validación.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	if (AreValidationLayersAvailable()) {
		Engine::GetLogger()->DebugLog("Capas de validación activas.");

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		Engine::GetLogger()->Log(LogLevel::WARNING, "No se ha encontrado soporte para las capas de validación.");
	}
#endif

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;
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

	swapchain->As<SwapchainVulkan>()->Create(Format::RGBA8_UNORM, *currentGpu->As<GpuVulkan>(), *window);
	Engine::GetLogger()->InfoLog("Creado el swapchain.");
}

void RendererVulkan::SetupDebugLogging() {
#ifdef OSK_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	OSK_ASSERT(func != nullptr, "No se puede iniciar la consola de capas de validación.");

	auto result = func(instance, &createInfo, nullptr, &debugConsole);
	OSK_ASSERT(result == VK_SUCCESS, "No se puede iniciar la consola de capas de validación.");

	Engine::GetLogger()->InfoLog("Capas de validación activas.");
#endif
}

void RendererVulkan::CreateSurface(const Window& window) {
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
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(instance, &count, devices.data());

	//Comprobar la compatibilidad de las GPUs.
	//Obtener una GPU compatible.
	std::vector<GpuVulkan::Info> gpus{};
	for (const auto& gpu : devices)
		gpus.push_back(GpuVulkan::Info::Get(gpu, surface));

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
	// Obtenemos el número de capas.
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Obtenemos las capas.
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//Capas de validación necesitadas.
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	for (auto layerName : validationLayers) {
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

void RendererVulkan::CreateSyncDevice() {
	syncDevice = currentGpu->As<GpuVulkan>()->CreateSyncDevice().GetPointer();

	syncDevice->As<SyncDeviceVulkan>()->SetDevice(*currentGpu->As<GpuVulkan>());
	syncDevice->As<SyncDeviceVulkan>()->SetSwapchain(*swapchain->As<SwapchainVulkan>());
}

void RendererVulkan::PresentFrame() {
	commandList->Close();

	syncDevice->As<SyncDeviceVulkan>()->FirstAwait();
	auto result = syncDevice->As<SyncDeviceVulkan>()->UpdateCurrentFrameIndex();
	if (result)
		return;
	syncDevice->As<SyncDeviceVulkan>()->Flush(*graphicsQueue->As<CommandQueueVulkan>() , *presentQueue->As<CommandQueueVulkan>(), *commandList->As<CommandListVulkan>());

	commandList->Reset();
	commandList->Start();
}
