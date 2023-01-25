#include "CommandQueueVulkan.h"

#include "GpuVulkan.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

CommandQueueVulkan::CommandQueueVulkan(CommandQueueSupport support, TIndex familyIndex, TIndex inFamilyIndex, const GpuVulkan& gpu)
	: ICommandQueue(support), familyIndex(familyIndex), inFamilyIndex(inFamilyIndex) {

	vkGetDeviceQueue(gpu.GetLogicalDevice(), familyIndex, inFamilyIndex, &queue);
}

VkQueue CommandQueueVulkan::GetQueue() const {
	return queue;
}

TIndex CommandQueueVulkan::GetQueueIndex() const {
	return inFamilyIndex;
}

TIndex CommandQueueVulkan::GetFamilyIndex() const {
	return familyIndex;
}
