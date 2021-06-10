#include "GpuImage.h"

namespace OSK::VULKAN {

	void GpuImage::Destroy() {
		if (image != VK_NULL_HANDLE) {
			vkDestroyImage(logicalDevice, image, nullptr);
			image = VK_NULL_HANDLE;
		}
		if (view != VK_NULL_HANDLE) {
			vkDestroyImageView(logicalDevice, view, nullptr);
			view = VK_NULL_HANDLE;
		}
		if (memory != VK_NULL_HANDLE) {
			vkFreeMemory(logicalDevice, memory, nullptr);
			memory = VK_NULL_HANDLE;
		}
	}

}