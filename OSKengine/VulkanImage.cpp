#include "VulkanImage.h"

namespace OSK::VULKAN {

	void VulkanImage::Destroy() {
		vkDestroyImage(*logicalDevice, Image, nullptr);
		vkDestroyImageView(*logicalDevice, View, nullptr);
		vkFreeMemory(*logicalDevice, Memory, nullptr);
	}

}