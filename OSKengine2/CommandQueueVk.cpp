#include "CommandQueueVk.h"

#include "GpuVk.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

CommandQueueVk::CommandQueueVk(CommandQueueSupport support, TIndex familyIndex, TIndex inFamilyIndex, const GpuVk& gpu)
	: ICommandQueue(support), familyIndex(familyIndex), inFamilyIndex(inFamilyIndex) {

	vkGetDeviceQueue(gpu.GetLogicalDevice(), familyIndex, inFamilyIndex, &queue);
}

VkQueue CommandQueueVk::GetQueue() const {
	return queue;
}

TIndex CommandQueueVk::GetQueueIndex() const {
	return inFamilyIndex;
}

TIndex CommandQueueVk::GetFamilyIndex() const {
	return familyIndex;
}
