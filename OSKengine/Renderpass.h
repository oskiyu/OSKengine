#pragma once

#include <vulkan/vulkan.h>

namespace OSK {

	class Renderpass {

	public:

		VkRenderPass VulkanRenderpass;

	private:

		VkFormat swapchainFormat;

	};

}