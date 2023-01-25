#include "GpuVulkan.h"

#include "Assert.h"
#include <set>
#include "CommandPoolVulkan.h"
#include "SyncDeviceVulkan.h"
#include <vulkan/vulkan.h>
#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

// Extensiones de la GPU que van a ser necesarias.
static DynamicArray<const char*> gpuExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

GpuVulkan::GpuVulkan(VkPhysicalDevice gpu, VkSurfaceKHR surface) : physicalDevice(gpu), surface(surface) {
	info = Info::Get(gpu, surface);
}

GpuVulkan::~GpuVulkan() {
	Close();
}

OwnedPtr<ICommandPool> GpuVulkan::CreateGraphicsCommandPool() {
	const QueueFamiles families = GetQueueFamilyIndices();
	const QueueFamily family = families.GetFamilies(CommandQueueSupport::GRAPHICS | CommandQueueSupport::COMPUTE | CommandQueueSupport::PRESENTATION).At(0);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = family.familyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el command pool.");

	auto output = new CommandPoolVulkan;
	output->SetCommandPool(commandPool);

	return output;
}

OwnedPtr<ICommandPool> GpuVulkan::CreateComputeCommandPool() {
	const QueueFamiles families = GetQueueFamilyIndices();
	const QueueFamily family = families.GetFamilies(CommandQueueSupport::GRAPHICS | CommandQueueSupport::COMPUTE | CommandQueueSupport::PRESENTATION).At(0);

	// @todo check por si no se puede usar una cola única.

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = family.familyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCommandPool commandPool = VK_NULL_HANDLE;

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

VkPhysicalDevice GpuVulkan::GetPhysicalDevice() const {
	return physicalDevice;
}

void GpuVulkan::CreateLogicalDevice() {
	const QueueFamiles queueFamilies = GetQueueFamilyIndices();
	std::set<uint32_t> uniqueQueueFamilies;

	const QueueFamily graphicsAndComputeFamily = queueFamilies.GetFamilies(CommandQueueSupport::GRAPHICS | CommandQueueSupport::COMPUTE).At(0);
	uniqueQueueFamilies.insert(graphicsAndComputeFamily.familyIndex);

	if (!EFTraits::HasFlag(graphicsAndComputeFamily.support, CommandQueueSupport::PRESENTATION))
		uniqueQueueFamilies.insert(queueFamilies.GetFamilies(CommandQueueSupport::PRESENTATION)[0].familyIndex);
	
	// Creación de las colas.
	DynamicArray<VkDeviceQueueCreateInfo> createInfos;
	float_t qPriority = 1.0f;
	for (auto& qFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo qCreateInfo{};
		qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qCreateInfo.queueFamilyIndex = qFamily;
		qCreateInfo.queueCount = 1;

		// Prioridades.
		qCreateInfo.pQueuePriorities = &qPriority;

		createInfos.Insert(qCreateInfo);
	}

	// Características que vamos a usar.
	VkPhysicalDeviceFeatures features{};
	features.samplerAnisotropy = VK_TRUE;
	features.tessellationShader = VK_TRUE; /// \todo check
	features.fillModeNonSolid = VK_TRUE; /// \todo check

	DynamicArray<VkExtensionProperties> extensionProperties = GetAvailableExtensions(physicalDevice);
	
	// RT
	if (info.IsRtCompatible()) {
		gpuExtensions.Insert(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

		gpuExtensions.Insert(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		gpuExtensions.Insert(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

		gpuExtensions.Insert(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
	}

	// Crear el logical device.
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = createInfos.GetSize();
	createInfo.pQueueCreateInfos = createInfos.GetData();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = gpuExtensions.GetSize();
	createInfo.ppEnabledExtensionNames = gpuExtensions.GetData();
	
	createInfo.pEnabledFeatures = nullptr;
	createInfo.pNext = &info.extendedFeatures;
	info.extendedFeatures.features = features;
	info.extendedFeatures.pNext = &info.dynamicRenderingFeatures;

	info.dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	info.dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	info.dynamicRenderingFeatures.pNext = nullptr;

	if (info.IsRtCompatible()) {
		info.dynamicRenderingFeatures.pNext = &info.rtAccelerationStructuresFeatures;

		info.rtAccelerationStructuresFeatures = {};
		info.rtAccelerationStructuresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
		info.rtAccelerationStructuresFeatures.accelerationStructure = VK_TRUE;
		info.rtAccelerationStructuresFeatures.pNext = &info.rtDeviceAddressFeatures;

		info.rtDeviceAddressFeatures = {};
		info.rtDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
		info.rtDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
		info.rtDeviceAddressFeatures.pNext = &info.rtPipelineFeatures;

		info.rtPipelineFeatures = {};
		info.rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
		info.rtPipelineFeatures.rayTracingPipeline = VK_TRUE;
		info.rtPipelineFeatures.pNext = &info.bindlessTexturesSets;

		info.bindlessTexturesSets.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		info.bindlessTexturesSets.runtimeDescriptorArray = VK_TRUE;
		info.bindlessTexturesSets.descriptorBindingVariableDescriptorCount = VK_TRUE;
	}

	// Crear el logical device.
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);

	// Error-handling.
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el logical device.");
}

VkDevice GpuVulkan::GetLogicalDevice() const {
	return logicalDevice;
}

const GpuVulkan::Info& GpuVulkan::GetInfo() const {
	return info;
}

QueueFamiles GpuVulkan::GetQueueFamilyIndices() const {
	QueueFamiles output{};

	// Número de familias.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	// Obtener las familias.
	auto queueFamilies = DynamicArray<VkQueueFamilyProperties>::CreateResizedArray(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.GetData());

	// Comprobar el soporte de distintos tipos de cola.
	int familyIndex = 0;
	for (const auto& q : queueFamilies) {
		QueueFamily family{};
		family.familyIndex = familyIndex;
		family.numQueues = q.queueCount;

		if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			family.support |= CommandQueueSupport::GRAPHICS;

		if (q.queueFlags & VK_QUEUE_COMPUTE_BIT)
			family.support |= CommandQueueSupport::COMPUTE;

		if (q.queueFlags & VK_QUEUE_TRANSFER_BIT)
			family.support |= CommandQueueSupport::TRANSFER;

		// Según el spec, si una familia soporta tanto GRAPHICS como COMPUTE, también debe soportar transfer, aunque no esté especificado explícitamente en el struct.
		if (EFTraits::HasFlag(family.support, CommandQueueSupport::COMPUTE) && EFTraits::HasFlag(family.support, CommandQueueSupport::GRAPHICS))
			family.support |= CommandQueueSupport::TRANSFER;

		// Soporte para presentación.
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface, &presentSupport);

		if (presentSupport)
			family.support |= CommandQueueSupport::PRESENTATION;

		output.families.Insert(family);

		familyIndex++;
	}

	return output;
}

OwnedPtr<ISyncDevice> GpuVulkan::CreateSyncDevice() {
	auto output = new SyncDeviceVulkan;

	return output;
}

GpuVulkan::Info GpuVulkan::Info::Get(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
	GpuVulkan::Info info{};

	info.physicalDevice = gpu;

	// Obtiene las propiedades de la gpu.
	vkGetPhysicalDeviceProperties(gpu, &info.properties);
	// Obtiene las características de la GPU.
	vkGetPhysicalDeviceFeatures(gpu, &info.features);

	// Obtiene las características de ray-tracing
	VkPhysicalDeviceProperties2 gpuProperties2{};
	gpuProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	gpuProperties2.properties = info.properties;
	gpuProperties2.pNext = &info.rtPipelineProperties;

	info.rtPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	info.rtPipelineProperties.pNext = &info.rtAccelerationStructuresProperites;

	info.rtAccelerationStructuresProperites.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

	vkGetPhysicalDeviceProperties2(gpu, &gpuProperties2);
	
	// Featrues
	info.extendedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	info.extendedFeatures.features = info.features;
	info.extendedFeatures.pNext = &info.rtPipelineFeatures;

	info.rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	info.rtPipelineFeatures.pNext = &info.rtAccelerationStructuresFeatures;
	info.rtAccelerationStructuresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	info.rtAccelerationStructuresFeatures.pNext = &info.rtDeviceAddressFeatures;
	info.rtDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	info.rtDeviceAddressFeatures.pNext = &info.dynamicRenderingFeatures;

	// Características para renderizado sin renderpasses
	info.dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	info.dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	info.dynamicRenderingFeatures.pNext = &info.bindlessTexturesSets;

	// Permitir arrays de recursos (para contener todas las texturas en un descriptor set).
	info.bindlessTexturesSets.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	info.bindlessTexturesSets.runtimeDescriptorArray = VK_TRUE;
	info.bindlessTexturesSets.descriptorBindingPartiallyBound = VK_TRUE;

	vkGetPhysicalDeviceFeatures2(gpu, &info.extendedFeatures);

	// Comprobar soporte de swapchain.
	bool swapchainSupported = false;

	//info.shapchainSupport = getSwapchainSupportDetails(gpu);
	//swapchainSupported = !info.shapchainSupport.presentModes.empty() && !info.shapchainSupport.formats.empty();

	//info.isSuitable = info.families.IsComplete() && checkGPUextensionSupport(gpu) && swapchainSupported && info.features.samplerAnisotropy;
	info.minAlignment = static_cast<TSize>(info.properties.limits.minUniformBufferOffsetAlignment);

	// ---------- SWAPCHAIN ------------------ //

	SwapchainSupportDetails shapchainSupport;

	//Obtener las capacidades.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &shapchainSupport.surfaceCapabilities);

	//Número de formatos soportados.
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

	//Obtener formatos soportados.
	shapchainSupport.formats.Resize(formatCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, shapchainSupport.formats.GetData());
	
	//Números de modos de presentación.
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);

	//Obtener modos de presentación.
	shapchainSupport.presentModes.Resize(presentModeCount);

	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &formatCount, shapchainSupport.presentModes.GetData());

	vkGetPhysicalDeviceMemoryProperties(gpu, &info.memoryProperties);

	info.swapchainSupportDetails = shapchainSupport;
	
	VkSampleCountFlags counts = info.properties.limits.framebufferColorSampleCounts & info.properties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_64_BIT; }
	else if (counts & VK_SAMPLE_COUNT_32_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_32_BIT; }
	else if (counts & VK_SAMPLE_COUNT_16_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_16_BIT; }
	else if (counts & VK_SAMPLE_COUNT_8_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_8_BIT; }
	else if (counts & VK_SAMPLE_COUNT_4_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_4_BIT; }
	else if (counts & VK_SAMPLE_COUNT_2_BIT) { info.maxMsaaSamples = VK_SAMPLE_COUNT_2_BIT; }

	if (info.dynamicRenderingFeatures.dynamicRendering == VK_TRUE)
		info.isSuitable = true;

	return info;
}

DynamicArray<VkExtensionProperties> GpuVulkan::GetAvailableExtensions(VkPhysicalDevice gpu) {
	DynamicArray<VkExtensionProperties> output;

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
	output.Resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, output.GetData());

	return output;
}

bool GpuVulkan::Info::IsRtCompatible() const {
	const auto availableExtensions = GpuVulkan::GetAvailableExtensions(physicalDevice);

	return (rtPipelineFeatures.rayTracingPipeline != 0) 
		&& (rtAccelerationStructuresFeatures.accelerationStructure != 0)
		&& (bindlessTexturesSets.descriptorBindingPartiallyBound == VK_TRUE) 
		&& (bindlessTexturesSets.runtimeDescriptorArray == VK_TRUE)
		
		&& GpuVulkan::IsExtensionPresent(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_KHR_SPIRV_1_4_EXTENSION_NAME, availableExtensions)
		&& GpuVulkan::IsExtensionPresent(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, availableExtensions);
}

bool GpuVulkan::IsExtensionPresent(const char* name, const DynamicArray<VkExtensionProperties>& extensions) {
	for (const auto& ext : extensions)
		if (strcmp(ext.extensionName, name) == 0)
			return true;

	return false;
}
