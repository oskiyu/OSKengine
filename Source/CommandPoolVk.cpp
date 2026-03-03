#include "CommandPoolVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "CommandListVk.h"
#include "GpuVk.h"
#include "Assert.h"
#include "QueueFamilyIndices.h"

#include "CommandListExceptions.h"
#include "RendererExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


template <VulkanTarget Target>
CommandPoolVk<Target>::CommandPoolVk(const GpuVk<Target>& device, const QueueFamily& queueType, GpuQueueType type) : ICommandPool(queueType.support, type), m_logicalDevice(device.GetLogicalDevice()) {
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

template <VulkanTarget Target>
CommandPoolVk<Target>::~CommandPoolVk() {
	vkDestroyCommandPool(
		m_logicalDevice,
		m_commandPool, 
		nullptr);
}

template <VulkanTarget Target>
UniquePtr<ICommandList> CommandPoolVk<Target>::CreateCommandList(const IGpu& device) {
	return CreateList(device, MAX_RESOURCES_IN_FLIGHT);
}

template <VulkanTarget Target>
UniquePtr<ICommandList> CommandPoolVk<Target>::CreateSingleTimeCommandList(const IGpu& device) {
	UniquePtr<ICommandList> list = CreateList(device, 1);
	list->_SetSingleTimeUse();

	return list;
}

template <VulkanTarget Target>
UniquePtr<ICommandList> CommandPoolVk<Target>::CreateList(const IGpu& device, USize32 numNativeLists) {
	return MakeUnique<CommandListVk<Target>>(
		*device.As<GpuVk<Target>>(),
		this);
}

template <VulkanTarget Target>
VkCommandPool CommandPoolVk<Target>::GetCommandPool() const {
	return m_commandPool;
}

#endif
