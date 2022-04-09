#include "GpuMemoryTypeVulkan.h"

#include "GpuMemoryTypes.h"
#include "GpuImageUsage.h"
#include <vulkan/vulkan.h>

#define OSK_CONTAINS_FLAG(a, b) EFTraits::HasFlag(a, b)

unsigned int OSK::GRAPHICS::GetGpuSharedMemoryTypeVulkan(GpuSharedMemoryType type) {
	switch (type) {

	case GpuSharedMemoryType::GPU_ONLY:
		return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	case GpuSharedMemoryType::GPU_AND_CPU:
		return VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	}

	return 0;
}

unsigned int OSK::GRAPHICS::GetGpuBufferUsageVulkan(GpuBufferUsage usage) {
	unsigned int flags = 0;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::UNIFORM_BUFFER))
		flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::VERTEX_BUFFER))
		flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::INDEX_BUFFER))
		flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::TRANSFER_SOURCE))
		flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::TRANSFER_DESTINATION))
		flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	return flags;
}

unsigned int OSK::GRAPHICS::GetGpuImageUsageVulkan(GpuImageUsage usage) {
	unsigned int flags = 0;

	if (EFTraits::HasFlag(usage, GpuImageUsage::COLOR))
		flags = flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::DEPTH_STENCIL))
		flags = flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::SAMPLED))
		flags = flags | VK_IMAGE_USAGE_SAMPLED_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::TRANSFER_SOURCE))
		flags = flags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::TRANSFER_DESTINATION))
		flags = flags | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	return flags;
}


unsigned int OSK::GRAPHICS::GetGpuImageAspectVulkan(GpuImageUsage usage) {
	unsigned int flags = 0;

	if (EFTraits::HasFlag(usage, GpuImageUsage::COLOR)
		|| EFTraits::HasFlag(usage, GpuImageUsage::SAMPLED))
		flags = flags | VK_IMAGE_ASPECT_COLOR_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::DEPTH_STENCIL))
		flags = flags | (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

	return flags;
}
