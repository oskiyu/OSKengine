#pragma once

#include <vulkan/vulkan.h>

struct VulkanImage {
	//
	VkImage Image;
	//
	VkDeviceMemory Memory;
	//
	VkImageView View;
};
