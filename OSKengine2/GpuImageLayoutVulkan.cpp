#include "GpuImageLayout.h"

#include "GpuImageLayoutVulkan.h"
#include <vulkan/vulkan.h>

VkImageLayout OSK::GetGpuImageLayoutVulkan(OSK::GpuImageLayout layout) {
	switch (layout) {
		case OSK::GpuImageLayout::UNDEFINED:
			return VK_IMAGE_LAYOUT_UNDEFINED;

		case OSK::GpuImageLayout::PRESENT:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		case OSK::GpuImageLayout::COLOR_ATTACHMENT:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		case OSK::GpuImageLayout::DEPTH_STENCIL_TARGET:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		case OSK::GpuImageLayout::DEPTH_STENCIL_READ_ONLY:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		case OSK::GpuImageLayout::SHADER_READ_ONLY:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		case OSK::GpuImageLayout::TRANSFER_SOURCE:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		case OSK::GpuImageLayout::TRANSFER_DESTINATION:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		default:
			return VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}
