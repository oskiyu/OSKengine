#include "DescriptorPoolVk.h"

#include "DescriptorPoolVk.h"
#include "DynamicArray.hpp"
#include "MaterialLayoutSlot.h"
#include "DescriptorLayoutVk.h"
#include "ShaderBindingTypeVk.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorPoolVk::DescriptorPoolVk(const DescriptorLayoutVk& layout, TSize maxSets) {
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

	VkResult result = vkCreateDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&poolInfo, nullptr, &pool);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el descriptor pool.");
}

DescriptorPoolVk::~DescriptorPoolVk() {
	vkDestroyDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		pool, nullptr);
}

VkDescriptorPool DescriptorPoolVk::GetPool() const {
	return pool;
}
