#include "GpuInfoVkAny.h"

#ifdef OSK_USE_VULKAN_BACKEND

#include "GpuVk.h"

using namespace OSK::GRAPHICS;

template <VulkanTarget Target>
bool GpuInfoVkAny<Target>::IsRtCompatible() const {
	if constexpr (Target == VulkanTarget::VK_1_0) {
		return false;
	}

	const auto availableExtensions = GpuVk<Target>::GetAvailableExtensions(physicalDevice);

	return (rtPipelineFeatures.rayTracingPipeline != 0)
		&& (rtAccelerationStructuresFeatures.accelerationStructure != 0)
		&& (bindlessTexturesSets.descriptorBindingPartiallyBound == VK_TRUE)
		&& (bindlessTexturesSets.runtimeDescriptorArray == VK_TRUE)

		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_SPIRV_1_4_EXTENSION_NAME, availableExtensions)
		&& GpuVk<Target>::IsExtensionPresent(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, availableExtensions)

		&& IsCompatibleWithBindless();
}

template <VulkanTarget Target>
bool GpuInfoVkAny<Target>::IsCompatibleWithBindless() const {
	if constexpr (Target == VulkanTarget::VK_1_0) {
		return false;
	}

	const auto availableExtensions = GpuVk<Target>::GetAvailableExtensions(physicalDevice);

	return
		// Para poder usar sets incompletos.
		bindlessTexturesSets.descriptorBindingPartiallyBound == VK_TRUE &&
		bindlessTexturesSets.runtimeDescriptorArray == VK_TRUE &&
		bindlessTexturesSets.shaderSampledImageArrayNonUniformIndexing == VK_TRUE &&
		GpuVk<Target>::IsExtensionPresent(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, availableExtensions);
}

template <VulkanTarget Target>
bool GpuInfoVkAny<Target>::IsCompatibleWithMeshShaders() const {
	if constexpr (Target == VulkanTarget::VK_1_0) {
		return false;
	}

	const auto availableExtensions = GpuVk<Target>::GetAvailableExtensions(physicalDevice);

	return
		// Para poder usar sets incompletos.
		meshShaders.meshShader == VK_TRUE &&
		meshShaders.taskShader == VK_TRUE &&
		GpuVk<Target>::IsExtensionPresent(VK_EXT_MESH_SHADER_EXTENSION_NAME, availableExtensions) &&
		GpuVk<Target>::IsExtensionPresent(VK_KHR_SPIRV_1_4_EXTENSION_NAME, availableExtensions) &&
		GpuVk<Target>::IsExtensionPresent(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, availableExtensions);
}

template <VulkanTarget Target>
GpuInfoVkAny<Target> GpuInfoVkAny<Target>::Get(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
	GpuInfoVkAny<Target> info{};
	info.isSuitable = true;

	info.physicalDevice = gpu;

	// Obtiene las propiedades de la gpu.
	vkGetPhysicalDeviceProperties(gpu, &info.properties);
	// Obtiene las características de la GPU.
	vkGetPhysicalDeviceFeatures(gpu, &info.features);

	if constexpr (Target == VulkanTarget::VK_LATEST) {
		// Obtiene las características de ray-tracing
		VkPhysicalDeviceProperties2 gpuProperties2{};
		gpuProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		gpuProperties2.properties = info.properties;
		gpuProperties2.pNext = &info.rtPipelineProperties;

		info.rtPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		info.rtPipelineProperties.pNext = &info.rtAccelerationStructuresProperites;

		info.rtAccelerationStructuresProperites.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

		vkGetPhysicalDeviceProperties2(gpu, &gpuProperties2);
	}

	// Featrues
	if constexpr (Target == VulkanTarget::VK_LATEST) {
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
		info.bindlessTexturesSets.pNext = &info.syncFeatures;

		// Permitir sincronización avanzada.
		info.syncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		info.syncFeatures.synchronization2 = VK_TRUE;
		info.syncFeatures.pNext = &info.meshShaders;

		// Permitir mesh shaders.
		info.meshShaders.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
		info.meshShaders.meshShader = VK_TRUE;
		info.meshShaders.taskShader = VK_TRUE;
		info.meshShaders.multiviewMeshShader = VK_FALSE;
		info.meshShaders.meshShaderQueries = VK_FALSE;
		info.meshShaders.primitiveFragmentShadingRateMeshShader = VK_FALSE;
		info.meshShaders.pNext = nullptr;

		vkGetPhysicalDeviceFeatures2(gpu, &info.extendedFeatures);
	}

	// Comprobar soporte de swapchain.
	bool swapchainSupported = false;

	//info.shapchainSupport = getSwapchainSupportDetails(gpu);
	//swapchainSupported = !info.shapchainSupport.presentModes.empty() && !info.shapchainSupport.formats.empty();

	//info.isSuitable = info.families.IsComplete() && checkGPUextensionSupport(gpu) && swapchainSupported && info.features.samplerAnisotropy;
	info.minAlignment = info.properties.limits.minUniformBufferOffsetAlignment;

	// ---------- SWAPCHAIN ------------------ //

	GpuInfoVkAny::SwapchainSupportDetails shapchainSupport;

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

	if constexpr (Target == VulkanTarget::VK_LATEST) {
		if (info.dynamicRenderingFeatures.dynamicRendering == VK_TRUE) {
			info.isSuitable = true;
		}
	}

	return info;
}

#endif
