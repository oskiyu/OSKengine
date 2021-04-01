#pragma once

#include <vulkan/vulkan.h>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Buffer de Vulkan.
	struct OSKAPI_CALL VulkanBuffer {

		friend class RenderAPI;

	public:

		VulkanBuffer() {}
		~VulkanBuffer();

		void Allocate(size_t size);
		void MapMemory();
		void MapMemory(size_t size, size_t offset);
		void Write(const void* data, size_t size, size_t offset = 0);
		void WriteMapped(const void* data, size_t size, size_t offset = 0);
		void UnmapMemory();
		void Free();

		size_t Size = 0;
		size_t DynamicSize = 0;

		//Buffer.
		VkBuffer Buffer = VK_NULL_HANDLE;

		//Memoria del buffer.
		VkDeviceMemory Memory = VK_NULL_HANDLE;

		uint32_t Alignment = 0;

	private:

		void Create(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps);

		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkBufferUsageFlags usage = VK_NULL_HANDLE;
		VkMemoryPropertyFlags properties = 0;
		VkPhysicalDeviceMemoryProperties gpuProps{};

		uint32_t getMemoryType(const uint32_t& memoryTypeFilter);

		void* mappedData = nullptr;
		bool isMapped = false;

	};

}