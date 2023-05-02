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

	// Información por cada set del layout.
	for (auto& i : layout.GetMaterialSlotLayout()->bindings) {
		VkDescriptorPoolSize size{};
		size.type = GetDescriptorTypeVk(i.second.type);

		// Número de descriptores en total.
		// Al tener varios recursos in-flight, debe haber un descriptor por cada frame in-flight.
		//
		// Para arrays / caso bindless, debe indicarse el número máximo de elementos del array.
		size.descriptorCount = Engine::GetRenderer()->GetSwapchainImagesCount() * maxSets;

		sizes.Insert(size);
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)sizes.GetSize();
	poolInfo.pPoolSizes = sizes.GetData();
	poolInfo.maxSets = Engine::GetRenderer()->GetSwapchainImagesCount();
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

	VkResult result = vkCreateDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&poolInfo, nullptr, &pool);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el descriptor pool.");
}

DescriptorPoolVk::~DescriptorPoolVk() {
	vkDestroyDescriptorPool(
		Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		pool, nullptr);
}

VkDescriptorPool DescriptorPoolVk::GetPool() const {
	return pool;
}
