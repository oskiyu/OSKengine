#include "CommandQueueVk.h"

#include "GpuVk.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

CommandQueueVk::CommandQueueVk(CommandQueueSupport support, UIndex32 familyIndex, UIndex32 inFamilyIndex, const GpuVk& gpu)
	: ICommandQueue(support), familyIndex(familyIndex), inFamilyIndex(inFamilyIndex) {

	vkGetDeviceQueue(gpu.GetLogicalDevice(), familyIndex, inFamilyIndex, &queue);
}

VkQueue CommandQueueVk::GetQueue() const {
	return queue;
}

UIndex32 CommandQueueVk::GetQueueIndex() const {
	return inFamilyIndex;
}

UIndex32 CommandQueueVk::GetFamilyIndex() const {
	return familyIndex;
}
