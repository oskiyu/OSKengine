#include "CommandListVulkan.h"

using namespace OSK;

const std::vector<VkCommandBuffer>& CommandListVulkan::GetCommandBuffers() const {
	return commandBuffers;
}

std::vector<VkCommandBuffer>* CommandListVulkan::GetCommandBuffers() {
	return &commandBuffers;
}
