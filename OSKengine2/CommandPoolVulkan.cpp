#include "CommandPoolVulkan.h"

#include <vulkan/vulkan.h>

#include "CommandListVulkan.h"
#include "GpuVulkan.h"
#include "Assert.h"

using namespace OSK;

OwnedPtr<ICommandList> CommandPoolVulkan::CreateCommandList(const IGpu& device) {
	auto list = new CommandListVulkan;

	list->GetCommandBuffers()->resize(numberOfImages);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(list->GetCommandBuffers()->size());

	VkResult result = vkAllocateCommandBuffers(device.As<GpuVulkan>()->GetLogicalDevice(), &allocInfo, list->GetCommandBuffers()->data());
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear la lista de comandos.");
	
	return list;
}

void CommandPoolVulkan::SetSwapchainCount(unsigned int count) {
	numberOfImages = count;
}

unsigned int CommandPoolVulkan::GetSwapchainCount() const {
	return numberOfImages;
}

void CommandPoolVulkan::SetCommandPool(VkCommandPool commandPool) {
	this->commandPool = commandPool;
}

VkCommandPool CommandPoolVulkan::GetCommandPool() const {
	return commandPool;
}
