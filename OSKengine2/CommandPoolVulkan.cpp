#include "CommandPoolVulkan.h"

#include <vulkan/vulkan.h>

#include "CommandListVulkan.h"
#include "GpuVulkan.h"
#include "Assert.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"

using namespace OSK;

CommandPoolVulkan::~CommandPoolVulkan() {
	vkDestroyCommandPool(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		commandPool, nullptr);
}

OwnedPtr<ICommandList> CommandPoolVulkan::CreateCommandList(const IGpu& device) {
	return CreateList(device, numberOfImages);
}

OwnedPtr<ICommandList> CommandPoolVulkan::CreateSingleTimeCommandList(const IGpu& device) {
	return CreateList(device, 1);
}

OwnedPtr<ICommandList> CommandPoolVulkan::CreateList(const IGpu& device, TSize numNativeLists) {
	auto list = new CommandListVulkan;

	list->GetCommandBuffers()->Reserve(numNativeLists);
	for (size_t i = 0; i < numNativeLists; i++)
		list->GetCommandBuffers()->Insert(VK_NULL_HANDLE);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(list->GetCommandBuffers()->GetSize());

	VkResult result = vkAllocateCommandBuffers(device.As<GpuVulkan>()->GetLogicalDevice(), &allocInfo, list->GetCommandBuffers()->GetData());
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
