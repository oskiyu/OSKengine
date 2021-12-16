#include "DescriptorSet.h"
#include "GraphicsPipeline.h"

namespace OSK {

	DescriptorSet::DescriptorSet(VkDevice logicalDevice, uint32_t swapchainCount) {
		this->logicalDevice = logicalDevice;
		this->swapchainCount = swapchainCount;
		this->shaderSet = shaderSet;

		vulkanDescriptorSets.resize(swapchainCount);
		descriptorWrites.resize(swapchainCount);
	}

	DescriptorSet::~DescriptorSet() {
		clear();
	}

	uint32_t DescriptorSet::GetBindingsCount() const {
		return bindingsCount;
	}

	DescriptorLayout* DescriptorSet::GetDescriptorLayout() const {
		return layout;
	}

	void DescriptorSet::Create(DescriptorLayout* layout, DescriptorPool* pool, bool allocate) {
		this->layout = layout;
		this->pool = pool;

		if (!allocate)
			return;

		std::vector<VkDescriptorSetLayout> layouts(swapchainCount, layout->vulkanDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool->vulkanDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainCount);
		allocInfo.pSetLayouts = layouts.data();

		VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, vulkanDescriptorSets.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor sets.");
	}

	void DescriptorSet::AddUniformBuffers(std::vector<SharedPtr<GpuDataBuffer>> buffers, uint32_t binding, size_t size) {
		for (uint32_t i = 0; i < vulkanDescriptorSets.size(); i++) {
			VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
			bufferInfo->buffer = buffers[i]->memorySubblock->GetNativeGpuBuffer();
			bufferInfo->offset = buffers[i]->memorySubblock->GetOffset();
			bufferInfo->range = size;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vulkanDescriptorSets[i];
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

		bindingsCount++;
	}

	void DescriptorSet::AddDynamicUniformBuffers(std::vector<SharedPtr<GpuDataBuffer>> buffers, uint32_t binding, size_t size) {
		for (uint32_t i = 0; i < vulkanDescriptorSets.size(); i++) {
			VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
			bufferInfo->buffer = buffers[i]->memorySubblock->GetNativeGpuBuffer();
			bufferInfo->offset = buffers[i]->memorySubblock->GetOffset();
			bufferInfo->range = size;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vulkanDescriptorSets[i];
			descriptorWrite.dstBinding = binding; //Binding
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			descriptorWrites[i].push_back(descriptorWrite);

			bufferInfos.push_back(bufferInfo);
		}

		bindingsCount++;
	}

	void DescriptorSet::AddImage(VULKAN::GpuImage* image, VkSampler sampler, uint32_t binding) {
		for (uint32_t i = 0; i < vulkanDescriptorSets.size(); i++) {
			VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
			imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo->imageView = image->view;
			imageInfo->sampler = sampler;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vulkanDescriptorSets[i];
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

		bindingsCount++;
	}

	void DescriptorSet::Update() {
		for (uint32_t i = 0; i < descriptorWrites.size(); i++) {
			vkUpdateDescriptorSets(logicalDevice, (uint32_t)descriptorWrites[i].size(), descriptorWrites[i].data(), 0, nullptr);
		}

		clear();
	}

	void DescriptorSet::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration, uint32_t setNumber) const {
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vulkanPipelineLayout, setNumber, 1, &vulkanDescriptorSets[iteration], 0, nullptr);
	}

	void DescriptorSet::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration, uint32_t setNumber, uint32_t dynamicOffset, uint32_t alignment) const {
		uint32_t finalOffset = dynamicOffset * alignment;

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vulkanPipelineLayout, setNumber, 1, &vulkanDescriptorSets[iteration], 1, &finalOffset);
	}
	
	void DescriptorSet::Reset() {
		clear();
		bindingsCount = 0;
	}

	void DescriptorSet::clear() {
		if (cleared)
			return;

		for (auto i : bufferInfos)
			delete i;
		for (auto i : imageInfos)
			delete i;

		imageInfos.clear();

		cleared = true;
	}

}