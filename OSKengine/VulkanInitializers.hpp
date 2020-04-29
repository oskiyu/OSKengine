#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Init {

	inline VkMemoryAllocateInfo memoryAllocateInfo() {
		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		return memAllocInfo;
	}

	inline VkImageMemoryBarrier imageMemoryBarrier() {
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		return imageMemoryBarrier;
	}

	inline VkBufferCreateInfo bufferCreateInfo(VkBufferUsageFlags usage, VkDeviceSize size) {
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.size = size;

		return bufferCreateInfo;
	}

	inline VkMappedMemoryRange mappedMemoryRange() {
		VkMappedMemoryRange mappedMemoryRAnge = {};
		mappedMemoryRAnge.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

		return mappedMemoryRAnge;
	}

	inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count) {
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = pool;
		commandBufferAllocateInfo.level = level;
		commandBufferAllocateInfo.commandBufferCount = count;

		return commandBufferAllocateInfo;
	}

	inline VkCommandBufferBeginInfo commandBufferBeginInfo() {
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		return info;
	}

	inline VkSubmitInfo submitInfo() {
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		return submitInfo;
	}

	inline VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0) {
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = flags;

		return info;
	}

}