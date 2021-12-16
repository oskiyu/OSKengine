#include "DescriptorPool.h"

#include "DescriptorLayout.h"

using namespace OSK;

DescriptorPool::DescriptorPool(VkDevice logicalDevice, uint32_t shapchainImageCount) : logicalDevice(logicalDevice), swapchainImageCount(shapchainImageCount) {

}

DescriptorPool::~DescriptorPool() {
	if (vulkanDescriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(logicalDevice, vulkanDescriptorPool, nullptr);
}

void DescriptorPool::AddBinding(VkDescriptorType type) {
	VkDescriptorPoolSize size{};
	size.type = type;
	size.descriptorCount = swapchainImageCount;

	descriptorPoolSizes.Insert(size);
}

void DescriptorPool::SetLayout(DescriptorLayout* layout) {
	for (auto& i : layout->descriptorLayoutBindings) {
		AddBinding(i.descriptorType);
	}
}

void DescriptorPool::Create(uint32_t maxSets) {
	for (auto& i : descriptorPoolSizes)
		i.descriptorCount *= maxSets;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)descriptorPoolSizes.GetSize();
	poolInfo.pPoolSizes = descriptorPoolSizes.GetData();
	poolInfo.maxSets = static_cast<uint32_t>(swapchainImageCount * maxSets);

	VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &vulkanDescriptorPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear descriptor pool.");
}
