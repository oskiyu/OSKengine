#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace OSK::VULKAN {

	struct VulkanDevice;

	struct VulkanImage;

	struct VulkanSwapchain;

	struct VulkanDescriptorSet {

		std::vector<VkDescriptorSet> DescriptorSets;

		void CreateDescriptorSet(const VulkanDevice& device, const VulkanImage& image, const uint32_t& uboSize, const VulkanSwapchain& swapchain);

	};

}
