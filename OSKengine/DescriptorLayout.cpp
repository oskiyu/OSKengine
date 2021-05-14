#include "DescriptorLayout.h"

#include "DescriptorPool.h"

#include <stdexcept>

namespace OSK {

	DescriptorLayout::DescriptorLayout(VkDevice logicalDevice) : logicalDevice(logicalDevice) {

	}

	DescriptorLayout::~DescriptorLayout() {
		if (vulkanDescriptorSetLayout != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(logicalDevice, vulkanDescriptorSetLayout, nullptr);
	}

	void DescriptorLayout::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stage;
		layoutBinding.pImmutableSamplers = nullptr;

		descriptorLayoutBindings.Insert(layoutBinding);
	}

	void DescriptorLayout::AddBinding(VkDescriptorType type, VkShaderStageFlags stage) {
		AddBinding(descriptorLayoutBindings.GetSize(), type, stage);
	}

	void DescriptorLayout::Create() {
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = descriptorLayoutBindings.GetSize();
		layoutInfo.pBindings = descriptorLayoutBindings.GetData();

		VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &vulkanDescriptorSetLayout);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor set layout.");
	}

}
