#include "VulkanInstance.h"

namespace OSK::VULKAN {

	void VulkanInstance::CreateInstance(const std::string& appName, const int32_t& appVersionMayor, const int32_t& appVersionMinor, const int32_t& appVersionRevision) {
		if (UseValidationLayers && !CheckValidationLayerSupport()) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "ERROR: capas de validación no disponibles.", __LINE__);
		}

		//struct con la información sobre la instancia, el engine y el juego
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; //Tipo de struct
		appInfo.pApplicationName = appName.c_str(); //Nombre del juego
		appInfo.applicationVersion = VK_MAKE_VERSION(appVersionMayor, appVersionMinor, appVersionRevision); //Versión del juego
		appInfo.pEngineName = OSK::ENGINE_NAME; //Nombre del engine
		appInfo.engineVersion = VK_MAKE_VERSION(OSK::ENGINE_VERSION_MAYOR_NUMERIC, OSK::ENGINE_VERSION_MINOR_NUMERIC, 0); //Versión del engine
		appInfo.apiVersion = VK_API_VERSION_1_1; //Versión de Vulkan a usar

		auto extensions = GetRequiredExtensions();

		//struct con las extensiones y capas de validación a usar
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; //Tipo de struct
		createInfo.pApplicationInfo = &appInfo; //La información del juego está en 'appInfo'
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); //Establecer el número de extensiones disponible
		createInfo.ppEnabledExtensionNames = extensions.data(); //Establecer el nombre de las extensiones disponibles
		if (UseValidationLayers) { //Si las capas de validación están permitidas, establecerlas
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size()); //Número de capas de validación
			createInfo.ppEnabledLayerNames = ValidationLayers.data(); //Capas de validación
		}
		else { //Si no están permitidas, esrtablecer que no se usen
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = NULL;
		}

		OSK::Logger::DebugLog("Extesiones soportadas: ");
		for (const auto& extension : extensions) {
			OSK::Logger::DebugLog(std::string(extension));
		}

		//Crear la instancia
		VkResult result = vkCreateInstance(&createInfo, NULL, &Instance); //Información del juego, custom allocator callbacks, instancia

		//Comprobar que todo ha ido bien
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "crear instancia", __LINE__);
		}
	}


	bool VulkanInstance::CheckValidationLayerSupport() {
		//Establecer y almacenar el número de capas de validación soportadas
		uint32_t layerCount; //Número de capas soportadas
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr); //Número de capas -> 'layerCount'
		std::vector<VkLayerProperties> avaiableLayers(layerCount); //Vector que almacena las capas disponibles
		vkEnumerateInstanceLayerProperties(&layerCount, avaiableLayers.data()); //Consulta las capas disponibles

		//Comprobar si las capas que necesitamos están soportadas
		for (const char* layerName : ValidationLayers) { // <- Capas necesitadas
			bool found = false; //¿Ha sido encontrada?

			//Comprobar si alguna de las capas está soprtada
			for (const auto& layerProperties : avaiableLayers) { // <- Capas soportadas

				//Comparar los nombres de las capa con la de la capa que necesitamos
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					found = true;
					break;
				}
			}

			//Si no ha sido encontrada, return false
			if (!found) {
				return false;
			}
		}

		//Si ha sido encontrada, return true
		return true;
	}


	void VulkanInstance::Destroy() {
		vkDestroyInstance(Instance, nullptr);
	}


	std::vector<const char*> VulkanInstance::GetRequiredExtensions() {
		//Extensiones y capas de validación a usar
		uint32_t glfwExtensionsCount = 0; //Número de extensiones
		const char** glfwExtensions; //Nombre de las extensiones

		//Obtener extensiones soportadas
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		//Extensiones de GLFW3 + algunas más
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

		//Si se usan capas de validación, añadir la extensión para el debug messenger ('VK_EXT_debug_utils')
		if (UseValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

}