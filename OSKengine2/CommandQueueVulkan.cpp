#include "CommandQueueVulkan.h"

#include "GpuVulkan.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

void CommandQueueVulkan::Create(TSize nativeQueueIndex, const GpuVulkan& gpu) {
	vkGetDeviceQueue(gpu.GetLogicalDevice(), nativeQueueIndex, 0, &queue);
}

VkQueue CommandQueueVulkan::GetQueue() const {
	return queue;
}
