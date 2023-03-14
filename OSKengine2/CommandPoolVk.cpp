#include "CommandPoolVk.h"

#include <vulkan/vulkan.h>

#include "CommandListVk.h"
#include "GpuVk.h"
#include "Assert.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

CommandPoolVk::~CommandPoolVk() {
	vkDestroyCommandPool(Engine::GetRenderer()->As<RendererVk>()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		commandPool, nullptr);
}

OwnedPtr<ICommandList> CommandPoolVk::CreateCommandList(const IGpu& device) {
	return CreateList(device, numberOfImages);
}

OwnedPtr<ICommandList> CommandPoolVk::CreateSingleTimeCommandList(const IGpu& device) {
	return CreateList(device, 1);
}

OwnedPtr<ICommandList> CommandPoolVk::CreateList(const IGpu& device, TSize numNativeLists) {
	auto list = new CommandListVk;

	list->GetCommandBuffers()->Reserve(numNativeLists);
	for (size_t i = 0; i < numNativeLists; i++)
		list->GetCommandBuffers()->Insert(VK_NULL_HANDLE);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(list->GetCommandBuffers()->GetSize());

	VkResult result = vkAllocateCommandBuffers(device.As<GpuVk>()->GetLogicalDevice(), &allocInfo, list->GetCommandBuffers()->GetData());
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear la lista de comandos.");

	return list;
}

void CommandPoolVk::SetSwapchainCount(unsigned int count) {
	numberOfImages = count;
}

unsigned int CommandPoolVk::GetSwapchainCount() const {
	return numberOfImages;
}

void CommandPoolVk::SetCommandPool(VkCommandPool commandPool) {
	this->commandPool = commandPool;
}

VkCommandPool CommandPoolVk::GetCommandPool() const {
	return commandPool;
}
