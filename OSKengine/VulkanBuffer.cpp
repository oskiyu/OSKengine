#include "VulkanBuffer.h"

namespace OSK {

	/*VulkanBuffer::VulkanBuffer(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps) {
		logicalDevice = device;
		this->usage = usage;
		this->properties = properties;
		this->gpuProps = gpuProps;
	}

	void VulkanBuffer::Alloc(const size_t& size) {
		this->Size = size;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &Buffer);
		if (result != VK_SUCCESS) {
			OSK_SHOW_ERROR("crear buffer.");
			//return OskResult::ERROR_CREATE_BUFFER;
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, Buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits);
		if (allocInfo.memoryTypeIndex < 0) {
			OSK_SHOW_ERROR("no se encuentra memoria compatible.");
			//return OskResult::ERROR_GPU_BUFFER_MEMORY_COMPATIBILITY;
		}

		result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &Memory);
		if (result != VK_SUCCESS) {
			OSK_SHOW_ERROR("alloc buffer memory.");
			//return OskResult::ERROR_ALLOC_BUFFER_MEMORY;
		}

		vkBindBufferMemory(logicalDevice, Buffer, Memory, 0);
	}

	void VulkanBuffer::Write(const void* data) {
		void* d = (void*)data;
		vkMapMemory(logicalDevice, Memory, 0, Size, 0, &d);
		memcpy((void*)data, data, Size);
		vkUnmapMemory(logicalDevice, Memory);
	}

	void VulkanBuffer::Free() {
		if (!inUse)
			return;

		vkDestroyBuffer(logicalDevice, Buffer, nullptr);
		vkFreeMemory(logicalDevice, Memory, nullptr);

		inUse = false;
	}
	*/
	VulkanBuffer::~VulkanBuffer() {
		//Free();
	}
	/*
	uint32_t VulkanBuffer::getMemoryType(const uint32_t& memoryTypeFilter) {
		for (uint32_t i = 0; i < gpuProps.memoryTypeCount; i++)
			if (memoryTypeFilter & (1 << i) && (gpuProps.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		return -1;
	}*/

}