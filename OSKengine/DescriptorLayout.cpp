#include "DescriptorLayout.h"

#include <stdexcept>

namespace OSK {

	DescriptorLayout::DescriptorLayout(VkDevice logicalDevice, uint32_t swapchainCount) {
		this->logicalDevice = logicalDevice;
		swapchainImageCount = swapchainCount;
	}

	DescriptorLayout::~DescriptorLayout() {
		clear();
	}

	void DescriptorLayout::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stage;
		layoutBinding.pImmutableSamplers = nullptr;

		descriptorLayoutBindings.push_back(layoutBinding);

		VkDescriptorPoolSize size{};
		size.type = type;
		size.descriptorCount = swapchainImageCount;

		descriptorPoolSizes.push_back(size);
	}

	void DescriptorLayout::Create(uint32_t maxSets) {
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = descriptorLayoutBindings.size();
		layoutInfo.pBindings = descriptorLayoutBindings.data();

		VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &VulkanDescriptorSetLayout);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor set layout.");

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = descriptorPoolSizes.size();
		poolInfo.pPoolSizes = descriptorPoolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swapchainImageCount * maxSets);

		result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &VulkanDescriptorPool);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor pool.");
	}

	void DescriptorLayout::clear() {
		vkDestroyDescriptorSetLayout(logicalDevice, VulkanDescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(logicalDevice, VulkanDescriptorPool, nullptr);
	}

}