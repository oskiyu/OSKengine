#pragma once

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

namespace OSK::VULKAN {

	struct VulkanDevice;

	struct VulkanSurface;

	struct VulkanCommandPool {

		VkCommandPool CommandPool;

		//Crea la command pool, en la que se guardan los buffers de los comandos
		void CreateCommandPool(const VulkanDevice& device, const VulkanSurface& surface);

		void DestroyCommandPool(const VulkanDevice& device);

	};

}