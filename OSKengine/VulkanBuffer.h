#pragma once

#include <vulkan/vulkan.h>

#include "OSKmacros.h"

namespace OSK::VULKAN {

	//Buffer de Vulkan.
	struct OSKAPI_CALL VulkanBuffer {
		//Buffer.
		VkBuffer Buffer;
		//Memoria del buffer.
		VkDeviceMemory Memory;
	};

}