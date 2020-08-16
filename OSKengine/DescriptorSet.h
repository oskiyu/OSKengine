#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "DescriptorLayout.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "GraphicsPipeline.h"

namespace OSK {

	class DescriptorSet {

		friend class VulkanRenderer;

	public:

		~DescriptorSet();

		void SetDescriptorLayout(DescriptorLayout* layout);

		void AddUniformBuffers(std::vector<VulkanBuffer> buffers, const uint32_t& binding, const size_t& size);
		void AddImage(VULKAN::VulkanImage* image, VkSampler sampler, const uint32_t& binding);

		void Create();

		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, const uint32_t& iteration) const;

		std::vector<VkDescriptorSet> VulkanDescriptorSets;

	private:

		DescriptorSet(VkDevice logicalDevice, const uint32_t& swapchainCount);

		void clear();

		DescriptorLayout* layout;
		std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites{};

		VkDevice logicalDevice;
		uint32_t swapchainCount;

		std::vector<VkDescriptorBufferInfo*> bufferInfos{};
		std::vector<VkDescriptorImageInfo*> imageInfos{};

		bool cleared = false;

	};

}