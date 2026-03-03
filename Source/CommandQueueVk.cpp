#include "CommandQueueVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "GpuVk.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

template <VulkanTarget Target>
CommandQueueVk<Target>::CommandQueueVk(QueueFamily family, UIndex32 indexInsideFamily, GpuQueueType queueType, const GpuVk<Target>& gpu)
	: ICommandQueue(queueType), m_family(family), m_queueIndex(indexInsideFamily) {

	vkGetDeviceQueue(
		gpu.GetLogicalDevice(), 
		family.familyIndex, 
		indexInsideFamily, 
		&m_queue);
}

template <VulkanTarget Target>
const QueueFamily& CommandQueueVk<Target>::GetFamily() const {
	return m_family;
}

template <VulkanTarget Target>
UIndex32 CommandQueueVk<Target>::GetQueueIndexInsideFamily() const {
	return m_queueIndex;
}

template <VulkanTarget Target>
VkQueue CommandQueueVk<Target>::GetQueue() const {
	return m_queue;
}

#endif
