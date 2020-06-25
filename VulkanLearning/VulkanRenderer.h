#pragma once

#include <vulkan/vulkan.h>

#include "VulkanBuffer.h"

class VulkanRenderer {

	/*void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop, Buffer* buffer) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(LogicalDevice, &bufferInfo, nullptr, &buffer->Buffer);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear buffer.");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(LogicalDevice, buffer->Buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, prop);

		result = vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, &buffer->Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc mem.");

		vkBindBufferMemory(LogicalDevice, buffer, &buffer->Memory, 0);
	}
	*/

};