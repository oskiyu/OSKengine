module;

#include <vulkan/vulkan.h>
#include <string>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>

#include "Assert.h"

export module OSKengine.Graphics.RendererVulkan;

export import OSKengine.Graphics.IRenderer;
export import OSKengine.WindowAPI;

import OSKengine.Memory.UniquePtr;
import OSKengine.Logger;
import OSKengine.Graphics.GpuVulkan;
import OSKengine.Assert;
import OSKengine.Graphics.CommandQueueVulkan;
import OSKengine.Graphics.QueueFamilyIndices;

export namespace OSK {

	/// <summary>
	/// Renderizador que utiliza la API Vulkan.
	/// </summary>
	class RendererVulkan : public IRenderer {

	public:

		~RendererVulkan() {
			Close();
		}

		void Initialize(const std::string& appName, const Version& version, const Window& window) override {
			CreateInstance(appName, version);

			if (AreValidationLayersAvailable())
				SetupDebugLogging();

			CreateSurface(window);
			ChooseGpu();
		}

		void Close() override {
			GetLogger()->InfoLog("Destruyendo el renderizador de Vulkan.");

			vkDestroyInstance(instance, nullptr);
		}

	private:

		void CreateInstance(const std::string& appName, const Version& version) {
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
				GetLogger()->DebugLog("VALIDATION LAYERS: ON.");

				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else {
				GetLogger()->Log(LogLevel::WARNING, "No se ha encontrado soporte para las capas de validación.");
			}
#endif

			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;

			GetLogger()->InfoLog("Extensiones del renderizador: ");
			for (const auto& i : extensions)
				GetLogger()->InfoLog("	" + std::string(i));

			GetLogger()->InfoLog("Capas de validación del renderizador: ");
			for (const auto& i : validationLayers)
				GetLogger()->InfoLog("	" + std::string(i));

			VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
			OSK_ASSERT(result == VK_SUCCESS, "Crear instancia de Vulkan." + std::to_string(result));
		}

		void SetupDebugLogging() {
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

			GetLogger()->InfoLog("Capas de validación activas.");
#endif
		}

		void CreateSurface(const Window& window) {
			auto result = glfwCreateWindowSurface(instance, window._GetGlfw(), nullptr, &surface);
			OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear la superficie. " + std::to_string(result));

			GetLogger()->InfoLog("Se ha creado correctamente la superficie de Vulkan.");
		}

		void ChooseGpu() {
			currentGpu = new GpuVulkan();

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
				gpus.push_back(GetGpuInfo(gpu));

			VkPhysicalDevice gpu = devices[0];
			GpuVulkan::Info info = gpus[0];
			vkGetPhysicalDeviceMemoryProperties(gpu, &info.memoryProperties);

			OSK_ASSERT(gpu != VK_NULL_HANDLE, "No hay ninguna GPU compatible con Vulkan.");

			GetLogger()->InfoLog("GPU elegida: " + std::string(info.properties.deviceName));

			currentGpu->As<GpuVulkan>()->SetPhysicalDevice(gpu);
			currentGpu->As<GpuVulkan>()->SetInfo(info);

			// --------------- Logical ----------------- //

			auto queueFamilyIndices = FindQueueFamilyIndices(gpu);

			std::vector<VkDeviceQueueCreateInfo> createInfos;
			std::set<uint32_t> uniqueQueueFamilies = {
				queueFamilyIndices.graphicsFamily.value(),
				queueFamilyIndices.presentFamily.value()
			};

			// Creación de las colas.
			float_t qPriority = 1.0f;
			for (auto& qFamily : uniqueQueueFamilies) {
				VkDeviceQueueCreateInfo qCreateInfo{};
				qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				qCreateInfo.queueFamilyIndex = qFamily;
				qCreateInfo.queueCount = 1;

				// Prioridades.
				qCreateInfo.pQueuePriorities = &qPriority;

				createInfos.push_back(qCreateInfo);
			}

			// Características que vamos a usar.
			VkPhysicalDeviceFeatures features{};
			features.samplerAnisotropy = VK_TRUE;

			// Crear el logical device.
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfos.size());
			createInfo.pQueueCreateInfos = createInfos.data();
			createInfo.pEnabledFeatures = &features;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(gpuExtensions.size());
			createInfo.ppEnabledExtensionNames = gpuExtensions.data();

			// Crear el logical device.
			VkDevice logicalDevice;
			VkResult result = vkCreateDevice(gpu, &createInfo, nullptr, &logicalDevice);

			// Error-handling.
			OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el logical device.");

			VkQueue graphicsQ;
			VkQueue presentQ;

			// Obtener las colas.
			vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQ);
			vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQ);

			graphicsQueue.SetQueue(graphicsQ);
			presentQueue.SetQueue(presentQ);

			currentGpu->As<GpuVulkan>()->SetLogicalDevice(logicalDevice);
		}

		QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice gpu) {
			QueueFamilyIndices indices{};

			// Número de colas.
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

			// Obtener las colas.
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilies.data());

			// Comprobar el soporte de distintos tipos de cola.
			int i = 0;
			for (const auto& q : queueFamilies) {
				if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphicsFamily = i;

				// Soporte para presentación.
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);

				if (presentSupport)
					indices.presentFamily = i;

				if (indices.IsComplete())
					break;

				i++;
			}

			return indices;
		}

		GpuVulkan::Info GetGpuInfo(VkPhysicalDevice gpu) const {
			GpuVulkan::Info info{};

			// Obtiene las propiedades de la gpu.
			vkGetPhysicalDeviceProperties(gpu, &info.properties);
			// Obtiene las características de la GPU.
			vkGetPhysicalDeviceFeatures(gpu, &info.features);

			// Comprobar soporte de swapchain.
			bool swapchainSupported = false;

			//info.shapchainSupport = getSwapchainSupportDetails(gpu);
			//swapchainSupported = !info.shapchainSupport.presentModes.empty() && !info.shapchainSupport.formats.empty();

			//info.isSuitable = info.families.IsComplete() && checkGPUextensionSupport(gpu) && swapchainSupported && info.features.samplerAnisotropy;
			info.minAlignment = info.properties.limits.minUniformBufferOffsetAlignment;

			return info;
		}

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

			LogLevel level = LogLevel::WARNING;

			switch (messageType) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				level = LogLevel::INFO;
				break;

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				level = LogLevel::WARNING;
				break;

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				level = LogLevel::ERROR;
				break;

			default:
				level = LogLevel::WARNING;
				break;
			}

			GetLogger()->Log(level, std::string(pCallbackData->pMessage) + "\n");

			return VK_FALSE;
		}

		bool AreValidationLayersAvailable() const {
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

		// Extensiones de la GPU que van a ser necesarias.
		const std::vector<const char*> gpuExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance instance;
		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debugConsole;

		CommandQueueVulkan graphicsQueue;
		CommandQueueVulkan presentQueue;

	};

}
