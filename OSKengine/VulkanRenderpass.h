#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace OSK::VULKAN {

	struct VulkanSwapchain;

	struct VulkanDevice;

	struct VulkanRenderpass {

		VkRenderPass RenderPass;


		void CreateRenderpass(const VulkanDevice& device, const VulkanSwapchain& swapchain);


		void Destroy(const VulkanDevice& device);


		VkFormat FindDepthFormat(const VulkanDevice& device);


		VkFormat FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	};

}