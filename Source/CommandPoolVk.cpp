#include "CommandPoolVk.h"

#include <vulkan/vulkan.h>

#include "CommandListVk.h"
#include "GpuVk.h"
#include "Assert.h"
#include "QueueFamilyIndices.h"

#include "CommandListExceptions.h"
#include "RendererExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


CommandPoolVk::CommandPoolVk(const GpuVk& device, const QueueFamily& queueType, GpuQueueType type) : ICommandPool(queueType.support, type), m_logicalDevice(device.GetLogicalDevice()) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueType.familyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(
		device.GetLogicalDevice(), 
		&poolInfo, 
		nullptr, 
		&m_commandPool);

	OSK_ASSERT(result == VK_SUCCESS, CommandPoolCreationException(result));
}

CommandPoolVk::~CommandPoolVk() {
	vkDestroyCommandPool(
		m_logicalDevice,
		m_commandPool, 
		nullptr);
}

OwnedPtr<ICommandList> CommandPoolVk::CreateCommandList(const IGpu& device) {
	return CreateList(device, NUM_RESOURCES_IN_FLIGHT);
}

OwnedPtr<ICommandList> CommandPoolVk::CreateSingleTimeCommandList(const IGpu& device) {
	OwnedPtr<ICommandList> list = CreateList(device, 1);
	list->_SetSingleTimeUse();

	return list;
}

OwnedPtr<ICommandList> CommandPoolVk::CreateList(const IGpu& device, USize32 numNativeLists) {
	return new CommandListVk(
		*device.As<GpuVk>(),
		*this);
}

VkCommandPool CommandPoolVk::GetCommandPool() const {
	return m_commandPool;
}
