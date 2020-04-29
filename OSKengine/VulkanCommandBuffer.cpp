#include "VulkanCommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

namespace OSK::VULKAN {

	void VulkanCommandBuffer::BeginSingleTimeCommand(const VulkanDevice& device, VulkanCommandPool* commandPool) {
		TargetComandPool = commandPool;

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = TargetComandPool->CommandPool;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(device.LogicalDevice, &allocInfo, &CommandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(CommandBuffer, &beginInfo);
	}

	void VulkanCommandBuffer::EndSingleTimeCommand(const VulkanDevice& device, VkQueue Q) {
		vkEndCommandBuffer(CommandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffer;

		vkQueueSubmit(Q, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(Q);

		vkFreeCommandBuffers(device.LogicalDevice, TargetComandPool->CommandPool, 1, &CommandBuffer);

		TargetComandPool = nullptr;
	}

}