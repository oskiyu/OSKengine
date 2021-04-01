#include "DescriptorPool.h"

#include "DescriptorLayout.h"

using namespace OSK;

DescriptorPool::~DescriptorPool() {
	if (VulkanDescriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(LogicalDevice, VulkanDescriptorPool, nullptr);
}

void DescriptorPool::AddBinding(VkDescriptorType type) {
	VkDescriptorPoolSize size{};
	size.type = type;
	size.descriptorCount = SwapchainImageCount;

	DescriptorPoolSizes.Insert(size);
}

void DescriptorPool::SetLayout(DescriptorLayout* layout) {
	for (auto& i : layout->DescriptorLayoutBindings) {
		AddBinding(i.descriptorType);
	}
}

void DescriptorPool::Create(uint32_t maxSets) {
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = DescriptorPoolSizes.GetSize();
	poolInfo.pPoolSizes = DescriptorPoolSizes.GetData();
	poolInfo.maxSets = static_cast<uint32_t>(SwapchainImageCount * maxSets);

	VkResult result = vkCreateDescriptorPool(LogicalDevice, &poolInfo, nullptr, &VulkanDescriptorPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear descriptor pool.");
}