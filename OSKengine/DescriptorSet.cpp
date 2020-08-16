#include "DescriptorSet.h"

namespace OSK {

	DescriptorSet::DescriptorSet(VkDevice logicalDevice, const uint32_t& swapchainCount) {
		this->logicalDevice = logicalDevice;
		this->swapchainCount = swapchainCount;

		VulkanDescriptorSets.resize(swapchainCount);
		descriptorWrites.resize(swapchainCount);
	}

	DescriptorSet::~DescriptorSet() {
		clear();
	}

	void DescriptorSet::SetDescriptorLayout(DescriptorLayout* layout) {
		this->layout = layout;

		std::vector<VkDescriptorSetLayout> layouts(swapchainCount, layout->VulkanDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = layout->VulkanDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainCount);
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, VulkanDescriptorSets.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor sets.");

	}

	void DescriptorSet::AddUniformBuffers(std::vector<VulkanBuffer> buffers, const uint32_t& binding, const size_t& size) {
		for (uint32_t i = 0; i < VulkanDescriptorSets.size(); i++) {
			VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
			bufferInfo->buffer = buffers[i].Buffer;
			bufferInfo->offset = 0;
			bufferInfo->range = size;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = VulkanDescriptorSets[i];
			descriptorWrite.dstBinding = binding; //Binding
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			descriptorWrites[i].push_back(descriptorWrite);

			bufferInfos.push_back(bufferInfo);
		}
	}

	void DescriptorSet::AddImage(VULKAN::VulkanImage* image, VkSampler sampler, const uint32_t& binding) {
		for (uint32_t i = 0; i < VulkanDescriptorSets.size(); i++) {
			VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
			imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo->imageView = image->View;
			imageInfo->sampler = sampler;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = VulkanDescriptorSets[i];
			descriptorWrite.dstBinding = binding; //Binding
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;

			descriptorWrites[i].push_back(descriptorWrite);

			imageInfos.push_back(imageInfo);
		}
	}

	void DescriptorSet::Create() {
		for (uint32_t i = 0; i < descriptorWrites.size(); i++) {
			vkUpdateDescriptorSets(logicalDevice, descriptorWrites[i].size(), descriptorWrites[i].data(), 0, nullptr);
		}

		clear();
	}

	void DescriptorSet::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, const uint32_t& iteration) const {
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->VulkanPipelineLayout, 0, 1, &VulkanDescriptorSets[iteration], 0, nullptr);
	}

	void DescriptorSet::clear() {
		if (cleared)
			return;

		for (auto& i : bufferInfos)
			delete i;
		for (auto& i : imageInfos)
			delete i;

		bufferInfos.clear();
		imageInfos.clear();

		cleared = true;
	}

}