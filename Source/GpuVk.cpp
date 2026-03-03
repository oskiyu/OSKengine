#include "GpuVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "Assert.h"
#include <set>
#include "CommandPoolVk.h"
#include "GpuImageSamplerVk.h"
#include "OSKengine.h"
#include "Logger.h"

#include "RendererExceptions.h"
#include "CommandListExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

template<VulkanTarget Target>
GpuVk<Target>::GpuVk(VkPhysicalDevice gpu, VkSurfaceKHR surface) : physicalDevice(gpu), surface(surface) {
	info = GpuInfoVkAny<Target>::Get(gpu, surface);
	_SetName(info.properties.deviceName);

	GpuMemoryAlignments alignments{};
	alignments.minVertexBufferAlignment = 0;
	alignments.minIndexBufferAlignment = 0;
	alignments.minUniformBufferAlignment = static_cast<USize32>(info.properties.limits.minUniformBufferOffsetAlignment);
	alignments.minStorageBufferAlignment = static_cast<USize32>(info.properties.limits.minStorageBufferOffsetAlignment);
	SetMinAlignments(alignments);
}

template<VulkanTarget Target>
GpuVk<Target>::~GpuVk() {
	Close();
}

template<VulkanTarget Target>
GpuMemoryUsageInfo GpuVk<Target>::GetMemoryUsageInfo() const {
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudget{};
	memoryBudget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
	memoryBudget.pNext = nullptr;

	VkPhysicalDeviceMemoryProperties2 memoryProperties{};
	memoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	memoryProperties.pNext = &memoryBudget;

	vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &memoryProperties);

	GpuMemoryUsageInfo output{};

	for (UIndex32 i = 0; i < memoryProperties.memoryProperties.memoryHeapCount; i++) {
		const GpuSharedMemoryType memoryType = memoryProperties.memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT
			? GpuSharedMemoryType::GPU_ONLY
			: GpuSharedMemoryType::GPU_AND_CPU;

		GpuHeapMemoryUsageInfo heapInfo{};
		heapInfo.maxCapacity = memoryBudget.heapBudget[i];
		heapInfo.usedSpace = memoryBudget.heapUsage[i];
		heapInfo.memoryType = memoryType;

		output.heapsInfo.Insert(heapInfo);

		if (memoryType == GpuSharedMemoryType::GPU_ONLY) {
			output.gpuOnlyMemoryInfo.maxCapacity += heapInfo.maxCapacity;
			output.gpuOnlyMemoryInfo.usedSpace += heapInfo.usedSpace;
		}
		else {
			output.gpuAndCpuMemoryInfo.maxCapacity += heapInfo.maxCapacity;
			output.gpuAndCpuMemoryInfo.usedSpace += heapInfo.usedSpace;
		}
	}

	return output;
}

template<VulkanTarget Target>
UniquePtr<ICommandPool> GpuVk<Target>::CreateGraphicsCommandPool() {
	const QueueFamiles families = GetQueueFamilyIndices();
	const QueueFamily family = families.GetFamilies(
		CommandsSupport::GRAPHICS | 
		CommandsSupport::COMPUTE | 
		CommandsSupport::PRESENTATION).AtCpy(0);

	return MakeUnique<CommandPoolVk<Target>>(*this, family, GpuQueueType::MAIN);
}

template<VulkanTarget Target>
UniquePtr<ICommandPool> GpuVk<Target>::CreateComputeCommandPool() {
	const auto families = GetQueueFamilyIndices().GetFamilies(
		CommandsSupport::GRAPHICS | 
		CommandsSupport::COMPUTE | 
		CommandsSupport::PRESENTATION);

	const QueueFamily family = families.At(0);

	return MakeUnique<CommandPoolVk<Target>>(
		*this,
		family,
		GpuQueueType::MAIN);
}

template<VulkanTarget Target>
std::optional<UniquePtr<ICommandPool>> GpuVk<Target>::CreateTransferOnlyCommandPool() {
	const auto families = GetQueueFamilyIndices().GetFamilies(CommandsSupport::TRANSFER);

	for (const auto& family : families) {
		if (family.support == CommandsSupport::TRANSFER) {
			return MakeUnique<CommandPoolVk<Target>>(*this, family, GpuQueueType::ASYNC_TRANSFER);
		}
	}

	return {};
}

template<VulkanTarget Target>
UniquePtr<IGpuImageSampler> GpuVk<Target>::CreateSampler(const GpuImageSamplerDesc& info) const {
	return MakeUnique<GpuImageSamplerVk<Target>>(info, this);
}

template<VulkanTarget Target>
VkSurfaceKHR GpuVk<Target>::GetSurface() const {
	return surface;
}

template<VulkanTarget Target>
void GpuVk<Target>::Close() {
	if (logicalDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(logicalDevice, nullptr);
		logicalDevice = VK_NULL_HANDLE;
	}
}

template<VulkanTarget Target>
VkPhysicalDevice GpuVk<Target>::GetPhysicalDevice() const {
	return physicalDevice;
}

template<VulkanTarget Target>
void GpuVk<Target>::CreateLogicalDevice() {
	const QueueFamiles families = GetQueueFamilyIndices();

	const auto graphicsAndComputeFamiliesList = families.GetFamilies(
		CommandsSupport::GRAPHICS | CommandsSupport::COMPUTE);
	const auto transferOnlyQueues = families.GetFamilies(CommandsSupport::TRANSFER);

	std::set<uint32_t> uniqueQueueFamilies;

	const QueueFamily graphicsAndComputeFamily = graphicsAndComputeFamiliesList.At(0);
	uniqueQueueFamilies.insert(graphicsAndComputeFamily.familyIndex);

	if (!EFTraits::HasFlag(graphicsAndComputeFamily.support, CommandsSupport::PRESENTATION)) {
		uniqueQueueFamilies.insert(families.GetFamilies(CommandsSupport::PRESENTATION).AtCpy(0).familyIndex);
	}
	
	for (const auto& family : transferOnlyQueues) {
		// Sólamente transfer.
		if (family.support == CommandsSupport::TRANSFER) {
			uniqueQueueFamilies.insert(family.familyIndex);
			break;
		}
	}

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

		createInfos.Insert(std::move(qCreateInfo));
	}

	// Características que vamos a usar.
	VkPhysicalDeviceFeatures features{};
	features.samplerAnisotropy = VK_TRUE;
	features.tessellationShader = VK_TRUE; /// \todo check
	features.fillModeNonSolid = VK_TRUE; /// \todo check

	DynamicArray<VkExtensionProperties> availableExtensions = GetAvailableExtensions(physicalDevice);
	DynamicArray<const char*> gpuExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Solo en LATEST
	if constexpr (Target == VulkanTarget::VK_LATEST) {
		gpuExtensions.Insert(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
	}

	// RT
	if (info.IsRtCompatible()) {
		gpuExtensions.Insert(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

		gpuExtensions.Insert(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		gpuExtensions.Insert(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

		gpuExtensions.Insert(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

		m_supportsRayTracing = true;
	} else

	// Bindless
	if (info.IsCompatibleWithBindless()) {
		gpuExtensions.Insert(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	}

	if (info.IsCompatibleWithMeshShaders()) {
		gpuExtensions.Insert(VK_EXT_MESH_SHADER_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		gpuExtensions.Insert(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
	}

	// Crear el logical device.
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(createInfos.GetSize());
	createInfo.pQueueCreateInfos = createInfos.GetData();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(gpuExtensions.GetSize());
	createInfo.ppEnabledExtensionNames = gpuExtensions.GetData();

	// Cadena con extensiones.
	if constexpr (Target == VulkanTarget::VK_LATEST) {
		createInfo.pEnabledFeatures = nullptr;
		createInfo.pNext = &info.extendedFeatures;
		info.extendedFeatures.features = features;
		info.extendedFeatures.pNext = &info.dynamicRenderingFeatures;
	}

	// Renderizado dinámico (solo en LATEST).
	if constexpr (Target == VulkanTarget::VK_LATEST) {
		info.dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
		info.dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
		info.dynamicRenderingFeatures.pNext = &info.syncFeatures;
	}

	// Sincronización avanzada (solo en LATEST).
	if constexpr (Target == VulkanTarget::VK_LATEST) {
		info.syncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		info.syncFeatures.synchronization2 = VK_TRUE;
		info.syncFeatures.pNext = nullptr;
	}


	// Storage image sin formato (necesario)
	info.features.shaderStorageImageWriteWithoutFormat = true;

	// Mesh shaders (solo en LATEST).
	if (info.IsCompatibleWithMeshShaders()) {
		info.syncFeatures.pNext = &info.meshShaders;

		info.meshShaders.meshShader = VK_TRUE;
		info.meshShaders.taskShader = VK_TRUE;
		info.meshShaders.multiviewMeshShader = VK_FALSE;
		info.meshShaders.meshShaderQueries = VK_FALSE;
		info.meshShaders.primitiveFragmentShadingRateMeshShader = VK_FALSE;

		info.meshShaders.pNext = nullptr;
	}

	// Trazado de rayos (solo en LATEST).
	if (info.IsRtCompatible()) {
		if (info.IsCompatibleWithMeshShaders()) {
			info.meshShaders.pNext = &info.rtAccelerationStructuresFeatures;
		}
		else {
			info.syncFeatures.pNext = &info.rtAccelerationStructuresFeatures;
		}

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
		info.bindlessTexturesSets.pNext = nullptr;
	}
	else if (info.IsCompatibleWithBindless()) {
		if (info.IsCompatibleWithMeshShaders()) {
			info.meshShaders.pNext = &info.rtAccelerationStructuresFeatures;
		}
		else {
			info.syncFeatures.pNext = &info.rtAccelerationStructuresFeatures;
		}

		info.bindlessTexturesSets.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		info.bindlessTexturesSets.runtimeDescriptorArray = VK_TRUE;
		info.bindlessTexturesSets.descriptorBindingVariableDescriptorCount = VK_TRUE;
		info.bindlessTexturesSets.pNext = nullptr;
	}
	
	// Crear el logical device.
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);

	// Error-handling.
	OSK_ASSERT(result == VK_SUCCESS, LogicalDeviceCreationException(result));
}

template<VulkanTarget Target>
VkDevice GpuVk<Target>::GetLogicalDevice() const {
	return logicalDevice;
}

template<VulkanTarget Target>
const GpuInfoVkAny<Target>& GpuVk<Target>::GetInfo() const {
	return info;
}

template<VulkanTarget Target>
QueueFamiles GpuVk<Target>::GetQueueFamilyIndices() const {
	QueueFamiles output{};

	// Número de familias.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	// Obtener las familias.
	auto queueFamilies = DynamicArray<VkQueueFamilyProperties>::CreateResized(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.GetData());

	// Comprobar el soporte de distintos tipos de cola.
	int familyIndex = 0;
	for (const auto& q : queueFamilies) {
		QueueFamily family{};
		family.familyIndex = familyIndex;
		family.numQueues = q.queueCount;

		if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			family.support |= CommandsSupport::GRAPHICS;

		if (q.queueFlags & VK_QUEUE_COMPUTE_BIT)
			family.support |= CommandsSupport::COMPUTE;

		if (q.queueFlags & VK_QUEUE_TRANSFER_BIT)
			family.support |= CommandsSupport::TRANSFER;

		// Según el spec, si una familia soporta tanto GRAPHICS como COMPUTE, también debe soportar transfer, aunque no esté especificado explícitamente en el struct.
		if (EFTraits::HasFlag(family.support, CommandsSupport::COMPUTE) && EFTraits::HasFlag(family.support, CommandsSupport::GRAPHICS))
			family.support |= CommandsSupport::TRANSFER;

		// Soporte para presentación.
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface, &presentSupport);

		if (presentSupport)
			family.support |= CommandsSupport::PRESENTATION;

		output.families.Insert(family);

		familyIndex++;
	}

	return output;
}

template <VulkanTarget Target>
DynamicArray<VkExtensionProperties> GpuVk<Target>::GetAvailableExtensions(VkPhysicalDevice gpu) {
	DynamicArray<VkExtensionProperties> output;

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
	output.Resize(extensionCount);
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, output.GetData());

	return output;
}

template <VulkanTarget Target>
bool GpuVk<Target>::IsExtensionPresent(const char* name, const DynamicArray<VkExtensionProperties>& extensions) {
	for (const auto& ext : extensions) {
		if (strcmp(ext.extensionName, name) == 0) {
			return true;
		}
	}

	return false;
}

#endif
