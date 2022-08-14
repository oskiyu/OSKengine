#include "CommandQueueVulkan.h"

#include "GpuVulkan.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

void CommandQueueVulkan::Create(TSize nativeQueueIndex, const GpuVulkan& gpu) {
	vkGetDeviceQueue(gpu.GetLogicalDevice(), nativeQueueIndex, 0, &queue);
	queueIndex = nativeQueueIndex;
}

VkQueue CommandQueueVulkan::GetQueue() const {
	return queue;
}

uint32_t CommandQueueVulkan::GetQueueIndex() const {
	return queueIndex;
}
