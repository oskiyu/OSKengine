#include "GpuVulkan.h"

#include <vector>
#include "Assert.h"
#include <set>
#include "CommandPoolVulkan.h"

using namespace OSK;

// Extensiones de la GPU que van a ser necesarias.
const std::vector<const char*> gpuExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

GpuVulkan::~GpuVulkan() {
	Close();
}

OwnedPtr<ICommandPool> GpuVulkan::CreateCommandPool() {
	QueueFamilyIndices indices = GetQueueFamilyIndices(surface);

	//Para la graphics q.
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCommandPool commandPool;

	VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el command pool.");

	auto output = new CommandPoolVulkan;
	output->SetCommandPool(commandPool);

	return output;
}

VkSurfaceKHR GpuVulkan::GetSurface() const {
	return surface;
}

void GpuVulkan::Close() {
	if (logicalDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(logicalDevice, nullptr);
		logicalDevice = VK_NULL_HANDLE;
	}
}

void GpuVulkan::SetPhysicalDevice(VkPhysicalDevice gpu) {
	physicalDevice = gpu;
}

VkPhysicalDevice GpuVulkan::GetPhysicalDevice() const {
	return physicalDevice;
}

void GpuVulkan::CreateLogicalDevice(VkSurfaceKHR surface) {
	auto queueFamilyIndices = GetQueueFamilyIndices(surface);

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
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);

	// Error-handling.
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el logical device.");

	this->surface = surface;
}

VkDevice GpuVulkan::GetLogicalDevice() const {
	return logicalDevice;
}

void GpuVulkan::SetInfo(const Info& info) {
	this->info = info;
}

const GpuVulkan::Info& GpuVulkan::GetInfo() const {
	return info;
}

QueueFamilyIndices GpuVulkan::GetQueueFamilyIndices(VkSurfaceKHR surface) const {
	QueueFamilyIndices indices{};

	// Número de colas.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	// Obtener las colas.
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	// Comprobar el soporte de distintos tipos de cola.
	int i = 0;
	for (const auto& q : queueFamilies) {
		if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		// Soporte para presentación.
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

GpuVulkan::Info GpuVulkan::Info::Get(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
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


	// ---------- SWAPCHAIN ------------------ //

	SwapchainSupportDetails shapchainSupport;

	//Obtener las capacidades.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &shapchainSupport.surfaceCapabilities);

	//Número de formatos soportados.
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

	//Obtener formatos soportados.
	shapchainSupport.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, shapchainSupport.formats.data());

	//Números de modos de presentación.
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);

	//Obtener modos de presentación.
	shapchainSupport.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &formatCount, shapchainSupport.presentModes.data());

	info.swapchainSupportDetails = shapchainSupport;

	return info;
}
