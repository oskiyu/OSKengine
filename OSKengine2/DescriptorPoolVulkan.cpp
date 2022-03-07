#include "DescriptorPoolVulkan.h"

#include "DescriptorPoolVulkan.h"
#include "DynamicArray.hpp"
#include "MaterialLayoutSlot.h"
#include "DescriptorLayoutVulkan.h"
#include "ShaderBindingTypeVulkan.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVulkan.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorPoolVulkan::DescriptorPoolVulkan(const DescriptorLayoutVulkan& layout, TSize maxSets) {
	DynamicArray<VkDescriptorPoolSize> sizes;

	for (auto& i : layout.GetMaterialSlotLayout()->bindings) {
		VkDescriptorPoolSize size{};
		size.type = GetDescriptorTypeVk(i.second.type);
		size.descriptorCount = Engine::GetRenderer()->GetSwapchainImagesCount();

		sizes.Insert(size);
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)sizes.GetSize();
	poolInfo.pPoolSizes = sizes.GetData();
	poolInfo.maxSets = Engine::GetRenderer()->GetSwapchainImagesCount() * maxSets;

	VkResult result = vkCreateDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), 
		&poolInfo, nullptr, &pool);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el descriptor pool.");
}

DescriptorPoolVulkan::~DescriptorPoolVulkan() {
	vkDestroyDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		pool, nullptr);
}

VkDescriptorPool DescriptorPoolVulkan::GetPool() const {
	return pool;
}
