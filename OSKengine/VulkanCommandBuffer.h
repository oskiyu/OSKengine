#pragma once

#include <vulkan/vulkan.h>

#include "VulkanCommandPool.h"

namespace OSK::VULKAN {

	struct VulkanDevice;
	
	struct VulkanCommandBuffer {

		VkCommandBuffer CommandBuffer;

		VulkanCommandPool* TargetComandPool;

		void BeginSingleTimeCommand(const VulkanDevice& device, VulkanCommandPool* commandPool);

		void EndSingleTimeCommand(const VulkanDevice& device, VkQueue Q);

	};
}