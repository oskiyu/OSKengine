#include "GpuImageLayout.h"

#include "GpuImageLayoutVk.h"
#include <vulkan/vulkan.h>

VkImageLayout OSK::GRAPHICS::GetGpuImageLayoutVk(GpuImageLayout layout) {
	switch (layout) {
		case GpuImageLayout::UNDEFINED:
			return VK_IMAGE_LAYOUT_UNDEFINED;

		case GpuImageLayout::PRESENT:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		case GpuImageLayout::COLOR_ATTACHMENT:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		case GpuImageLayout::DEPTH_STENCIL_TARGET:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		case GpuImageLayout::DEPTH_STENCIL_READ_ONLY:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		case GpuImageLayout::SAMPLED:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		case GpuImageLayout::TRANSFER_SOURCE:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		case GpuImageLayout::TRANSFER_DESTINATION:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		case GpuImageLayout::GENERAL:
			return VK_IMAGE_LAYOUT_GENERAL;

		default:
			return VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}
