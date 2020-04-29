#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	struct VulkanDevice;

	struct VulkanCommandBuffer;

	struct VulkanCommandPool;

	struct VulkanBuffer {

		VkBuffer Buffer;

		VkDeviceMemory BufferMemory;

		VkDeviceSize BufferSize;

		void CreateBuffer(const VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);


		void CopyBufferFrom(const VulkanDevice& device, VulkanCommandPool* commandPool, const VulkanBuffer& sourceBuffer, const size_t& size, VkQueue Q);


		void DestroyBuffer(const VulkanDevice& device);

	};

}

/*
struct VulkanBuffer {
	VkDevice device;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo descriptor;
	VkDeviceSize size = 0;
	VkDeviceSize alignment = 0;
	void* mapped = nullptr;

	VkBufferUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryPropertyFlags;

	//Mapea un rango de memoria al buffer.
	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
		return vkMapMemory(device, memory, offset, size, 0, &mapped);
	}

	//Desmapea el rango de memoria.
	void unmap() {
		if (mapped) {
			vkUnmapMemory(device, memory);
			mapped = nullptr;
		}
	}

	//Attach allocated memory block to the buffer
	VkResult bind(VkDeviceSize offset = 0) {
		return vkBindBufferMemory(device, buffer, memory, offset);
	}

	//Default descriptor del buffer
	void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
		descriptor.offset = offset;
		descriptor.buffer = buffer;
		descriptor.range = size;
	}

	//Copia los datos al buffer
	void copyTo(void* data, VkDeviceSize size) {
		memcpy(mapped, data, size);
	}

	//Flush memory -> visible to device
	VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;

		return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
	}

	VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;

		return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
	}

	void destroy() {
		if (buffer)
			vkDestroyBuffer(device, buffer, nullptr);
		if (memory)
			vkFreeMemory(device, memory, nullptr);
	}
};
*/