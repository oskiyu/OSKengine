#include "VulkanDescriptorSet.h"

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanSwapchain.h"
#include "VulkanBuffer.h"

namespace OSK::VULKAN {

	void VulkanDescriptorSet::CreateDescriptorSet(const VulkanDevice& device, const VulkanImage& image, const uint32_t& uboSize, const VulkanSwapchain& swapchain) {
		std::vector<VkDescriptorSetLayout> layouts(swapchain.SwapchainImages.size(), swapchain.DescriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = swapchain.DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain.SwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		DescriptorSets.resize(swapchain.SwapchainImages.size());

		VkResult result = vkAllocateDescriptorSets(device.LogicalDevice, &allocInfo, DescriptorSets.data());

		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: alloc descriptor sets");
		}

		for (size_t i = 0; i < swapchain.SwapchainImages.size(); i++) {
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = swapchain.UniformBuffers[i].Buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = uboSize;

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = image.ImageView;
			imageInfo.sampler = image.ImageSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = NULL; // Optional
			descriptorWrites[0].pTexelBufferView = NULL; // Optional

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = NULL;
			descriptorWrites[1].pImageInfo = &imageInfo; // Optional
			descriptorWrites[1].pTexelBufferView = NULL; // Optional

			vkUpdateDescriptorSets(device.LogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, NULL);
		}
	}

}