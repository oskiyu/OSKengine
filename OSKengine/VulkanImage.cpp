#include "VulkanImage.h"

namespace OSK::VULKAN {

	void GPUImage::Destroy() {
		if (Image != VK_NULL_HANDLE) {
			vkDestroyImage(*logicalDevice, Image, nullptr);
			Image = VK_NULL_HANDLE;
		}
		if (View != VK_NULL_HANDLE) {
			vkDestroyImageView(*logicalDevice, View, nullptr);
			View = VK_NULL_HANDLE;
		}
		if (Memory != VK_NULL_HANDLE) {
			vkFreeMemory(*logicalDevice, Memory, nullptr);
			Memory = VK_NULL_HANDLE;
		}
	}

}