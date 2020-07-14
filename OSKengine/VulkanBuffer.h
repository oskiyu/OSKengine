#pragma once

#include <vulkan/vulkan.h>

#include "OSKmacros.h"

namespace OSK::VULKAN {

	struct OSKAPI_CALL VulkanBuffer {
		//Buffer.
		VkBuffer Buffer;
		//Memoria del buffer.
		VkDeviceMemory Memory;
		//Tamaño del buffer.
		//VkDeviceSize Size;
		//Uso del buffer.
		//VkBufferUsageFlags Usage;
		//Sharing mode.
		//VkSharingMode SharingMode;
		//Memory type index.
		//uint32_t MemoryTypeIndex;
	};

}