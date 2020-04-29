#include "VulkanDevice.h"

#include "VulkanBuffer.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

namespace OSK::VULKAN {

	void VulkanDevice::CreateLogicalDevice(const VulkanInstance& instance, VulkanSurface& surface, VkQueue& graphicsQ) {
		//Especificar que colas han de crearse para una queueFamily: una que tiene capacidades gráficas
		//Las FamilyIndices que se van a usar
		QueueFamilyIndices indices = FindQueueFamilies(surface, PhysicalDevice);

		//createInfo para las colas
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		//Colas a usar
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.GraphicsFamily.value(),
			indices.PresentFamily.value()
		};

		//Prioridad de la(s) cola(s)
		float queuePriority = 1.0f;

		//Crear las colas
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; //Tipo de struct
			queueCreateInfo.queueFamilyIndex = queueFamily; //Los índices de las colas
			queueCreateInfo.queueCount = 1; //Número de colas
			queueCreateInfo.pQueuePriorities = &queuePriority; //Prioridad de la cola
			queueCreateInfos.push_back(queueCreateInfo); //Incluye este createInfo en las colas
		}

		//Establecer que características de la GPU se van a usar
		VkPhysicalDeviceFeatures deviceFeatures = {};

		deviceFeatures.samplerAnisotropy = VK_TRUE;

		//struct que proveerá la información del logical device
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; //Tipo de struct
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()); //Número de colas (y de createInfo's)
		createInfo.pQueueCreateInfos = queueCreateInfos.data(); //Informaciones de las colas que necesitamos
		createInfo.pEnabledFeatures = &deviceFeatures; //Características de la GPU que vamos a usar

		//Establecer las extensiones y capas de validación que se van a usar
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (instance.UseValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(instance.ValidationLayers.size());
			createInfo.ppEnabledLayerNames = instance.ValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		//Inicializar el logical device
		VkResult result = vkCreateDevice(PhysicalDevice, &createInfo, NULL, &LogicalDevice); //'result' = resultado de haber intentado inicializar el logical device
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "create logical device", __LINE__);
		}

		//Guardar el handle de la cola gráfica en 'GraphicsQueue'
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamily.value(), 0, &graphicsQ);

		//Guardar el handle de la cola de superficie en 'SurfaceQueue'
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamily.value(), 0, &surface.SurfaceQueue);
	}


	void VulkanDevice::Destroy() {
		vkDestroyDevice(LogicalDevice, nullptr);
	}


	void VulkanDevice::PickPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface) {
		//Comprobar que GPU's tiene el PC (que sean compatibles con Vulkan)
		uint32_t deviceCount = 0; //Número de gráficas
		vkEnumeratePhysicalDevices(instance.Instance, &deviceCount, NULL); //Enlista las GPU's del PC

		//Si el PC no dispone de GPU, throw error
		if (deviceCount == 0) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "PickPhysicalDevica", __LINE__);
		}

		//Guardar los handles de las GPU's en un std::vector
		std::vector<VkPhysicalDevice> devices(deviceCount); //Vector en el que se almaccenan los handles de las GPU's
		vkEnumeratePhysicalDevices(instance.Instance, &deviceCount, devices.data()); //Guardar los handles en el std::vector 'devices'

#ifdef OSK_DEBUG
		VkPhysicalDeviceProperties gpuProperties = {};
		for (int i = 0; i < devices.size(); i++) {
			vkGetPhysicalDeviceProperties(devices[i], &gpuProperties);
			OSK::Logger::DebugLog("GPU: " + std::string(gpuProperties.deviceName));
			OSK::Logger::DebugLog("GPU vendor: " + std::to_string(gpuProperties.vendorID));
			OSK::Logger::DebugLog("API version: " + std::to_string(gpuProperties.apiVersion));
			OSK::Logger::DebugLog("Drivers: " + std::to_string(gpuProperties.driverVersion));
		}
#endif

		//Comprobar que GPU's son compatibles con lo que queremos hacer, y escoger una
		for (const auto& device : devices) {
			if (IsPhysicalDeviceSuitable(surface, device)) {
				PhysicalDevice = device;
				break;
			}
		}
		Logger::DebugLog("A");

		//Si no se ha conseguido encontrar una GPU compatible, throw error
		if (PhysicalDevice == VK_NULL_HANDLE) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "GPU not compatible", __LINE__);
		}
	}


	bool VulkanDevice::IsPhysicalDeviceSuitable(const VulkanSurface& surface, VkPhysicalDevice physicalDevice) {
		//Obtener las QueueFamily soportadas por la GPU
		QueueFamilyIndices indices = FindQueueFamilies(surface, physicalDevice);

		//Comprobar que la GPU soporta las extensiones necesarias
		bool extensionsSupported = CheckExtensionSupport(physicalDevice);

		//Comprobar que la gráfica soporta el swapChain
		bool swapchainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);
			swapchainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		//Demás
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

		//Ver si 'device' lo soporta todo
		return indices.GraphicsFamily.has_value() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
	}


	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VulkanSurface surface, VkPhysicalDevice physicalDevice) const {

		//Índices de las QueueFamily disponibles
		QueueFamilyIndices indices;

		//Comprobar las QUeueFamily disponibles
		uint32_t queueFamilyCount = 0; //Número de familias
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL); //Obtener las familias disponibles

		//Almacena las familias disponibles en el std::vector 'queueFamilies'
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount); //Almacena las familias
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data()); //Almacena las familias en 'queueFamilies'

		//Número de familias que soportan 'VK_QUEUE_GRAPHICS_BIT'
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {

			//Comprobar que esta familyQueue soporta 'VK_QUEUE_GRAPHICS_BIT'
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.GraphicsFamily = i;
			}

			//Comprobar que esta familyQueue soporta el renderizado en una superficie
			VkBool32 surfaceSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface.Surface, &surfaceSupport);

			//Si soporta la superficie de renderizado, se indica en los índices
			if (queueFamily.queueCount > 0 && surfaceSupport) {
				indices.PresentFamily = i;
			}

			//Si ya se ha encontrado una familia que soporte 'VK_QUEUE_GRAPHICS_BIT', entonces break
			if (indices.IsComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}
	

	SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface surface) {
		//Struct que almacena la información sobre las capacidades de la GPU para efectuar el swapChain
		SwapChainSupportDetails details;

		//Obtiene las capacidades de la GPU para efectuar el swapChain
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Surface, &details.capabilities);

		//Comprobar que formatos de swapChain están soortados por la GPU
		uint32_t formatCount = 0; //Número de formatos soportados
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, NULL); //Obtiene la información sobre los formatos soportados

		//Si hay al menos un formato soportado, se añade(n) al struct que almacena esta información
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, details.formats.data());
		}

		//Comprobar que modos de presentación están soortados por la GPU
		uint32_t presentModeCount; //Número de modos soportados
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, NULL); //Obtiene la información sobre los modos soportados

		//Si hay al menos un modo soportado, se añade(n) al struct que almacena esta información
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, details.presentModes.data());
		}


		return details;
	}


	uint32_t VulkanDevice::FindMemoryType(uint32_t type, VkMemoryPropertyFlags properties) const {
		//Propiedades de la memoria de la GPU
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &memoryProperties);

		//Encontrar un tipo de memoria compatible con la GPU
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if (type & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		//Error handling
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "no se puede encontrar memoria en la GPU", __LINE__);
		throw std::runtime_error("ERROR: no se puede encontrar memoria en la GPU");

		return 0;
	}


	bool VulkanDevice::CheckExtensionSupport(VkPhysicalDevice device) {

		//Obtiene las extensiones necesarias para el juego
		uint32_t extensionCount = 0; //Número de extensiones soportdas
		vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL); //Obtener información sobre las extensiones que soporta la GPU

		//Almacena la información sobre las extensiones soportadas
		std::vector<VkExtensionProperties> avaiableExtensions(extensionCount); //std::vector que almacena las extensiones soportadas
		vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, avaiableExtensions.data()); //Almacena los datos en 'avaiableExtensions'

		//"lista" con los nombres de todas las extensiones necesitadas
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		//Comprueba que las extensiones están soportadas y las quita de la lista
		for (const auto& extension : avaiableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		//Devuelve 'true' si no hay ninguna extensión necesitada sin ser soportada
		return requiredExtensions.empty();
	}


}