#pragma once

#include <vulkan/vulkan.h>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Buffer de Vulkan.
	struct OSKAPI_CALL VulkanBuffer {

		friend class VulkanRenderer;

	public:

		VulkanBuffer() {}
		~VulkanBuffer();

		/*void Alloc(const size_t& size);
		void Write(const void* data);
		void Free();

		size_t Size = 0;*/

		//Buffer.
		VkBuffer Buffer = VK_NULL_HANDLE;

		//Memoria del buffer.
		VkDeviceMemory Memory = VK_NULL_HANDLE;

	private:

	/*	VulkanBuffer(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps);

		VkDevice logicalDevice;
		VkBufferUsageFlags usage;
		VkMemoryPropertyFlags properties;
		VkPhysicalDeviceMemoryProperties gpuProps;

		uint32_t getMemoryType(const uint32_t& memoryTypeFilter);

		bool inUse = false;*/

	};

}