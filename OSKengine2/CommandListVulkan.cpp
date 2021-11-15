#include "CommandListVulkan.h"

#include <vulkan/vulkan.h>

using namespace OSK;

const std::vector<VkCommandBuffer>& CommandListVulkan::GetCommandBuffers() const {
	return commandBuffers;
}

std::vector<VkCommandBuffer>* CommandListVulkan::GetCommandBuffers() {
	return &commandBuffers;
}

void CommandListVulkan::Reset() {
	for (auto i : commandBuffers)
		vkResetCommandBuffer(i, 0);
}

void CommandListVulkan::Start() {
	static VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0, 
		NULL
	};

	for (auto i : commandBuffers)
		VkResult result = vkBeginCommandBuffer(i, &beginInfo);
}

void CommandListVulkan::Close() {
	
}
