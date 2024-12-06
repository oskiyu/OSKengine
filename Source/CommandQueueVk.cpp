#include "CommandQueueVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "GpuVk.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

CommandQueueVk::CommandQueueVk(QueueFamily family, UIndex32 indexInsideFamily, GpuQueueType queueType, const GpuVk& gpu)
	: ICommandQueue(family, queueType, indexInsideFamily) {

	vkGetDeviceQueue(
		gpu.GetLogicalDevice(), 
		family.familyIndex, 
		indexInsideFamily, 
		&m_queue);
}

VkQueue CommandQueueVk::GetQueue() const {
	return m_queue;
}

#endif
