#include "DescriptorLayout.h"

#include "DescriptorPool.h"

#include <stdexcept>

namespace OSK {

	DescriptorLayout::~DescriptorLayout() {
		if (VulkanDescriptorSetLayout != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(LogicalDevice, VulkanDescriptorSetLayout, nullptr);
	}

	void DescriptorLayout::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stage;
		layoutBinding.pImmutableSamplers = nullptr;

		DescriptorLayoutBindings.Insert(layoutBinding);
	}

	void DescriptorLayout::AddBinding(VkDescriptorType type, VkShaderStageFlags stage) {
		AddBinding(DescriptorLayoutBindings.GetSize(), type, stage);
	}

	void DescriptorLayout::Create() {
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = DescriptorLayoutBindings.GetSize();
		layoutInfo.pBindings = DescriptorLayoutBindings.GetData();

		VkResult result = vkCreateDescriptorSetLayout(LogicalDevice, &layoutInfo, nullptr, &VulkanDescriptorSetLayout);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear descriptor set layout.");
	}

}
