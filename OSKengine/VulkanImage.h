#pragma once

#include <vulkan/vulkan.h>
#include "OSKmacros.h"

namespace OSK::VULKAN {

	struct OSKAPI_CALL VulkanImage {
		//
		VkImage Image;
		//
		VkDeviceMemory Memory;
		//
		VkImageView View;
	};

}