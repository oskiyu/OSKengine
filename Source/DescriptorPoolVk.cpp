#include "DescriptorPoolVk.h"

#include "DescriptorPoolVk.h"
#include "DynamicArray.hpp"
#include "MaterialLayoutSlot.h"
#include "DescriptorLayoutVk.h"
#include "ShaderBindingTypeVk.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include "MaterialExceptions.h"

#include <vulkan/vulkan.h>

#include "BindlessLimits.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorPoolVk::DescriptorPoolVk(const DescriptorLayoutVk& layout, USize32 maxSets) {
	const bool isBindless = Engine::GetRenderer()->GetGpu()->SupportsBindlessResources();

	DynamicArray<VkDescriptorPoolSize> sizes;

	// Informaci�n por cada set del layout.
	for (auto& [name, binding] : layout.GetMaterialSlotLayout()->bindings) {
		VkDescriptorPoolSize size{};
		size.type = GetDescriptorTypeVk(binding.type);

		// N�mero de descriptores en total.
		// Al tener varios recursos in-flight, debe haber un descriptor por cada frame in-flight.
		//
		// Para arrays / caso bindless, debe indicarse el n�mero m�ximo de elementos del array.
		size.descriptorCount = Engine::GetRenderer()->GetSwapchainImagesCount() * maxSets;
		if (isBindless) {
			size.descriptorCount *= MAX_BINDLESS_RESOURCES;
		}

		sizes.Insert(size);
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.GetSize());
	poolInfo.pPoolSizes = sizes.GetData();
	poolInfo.maxSets = Engine::GetRenderer()->GetSwapchainImagesCount() * maxSets;
	poolInfo.flags = isBindless ? VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT : 0;

	VkResult result = vkCreateDescriptorPool(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&poolInfo, nullptr, &pool);
	OSK_ASSERT(result == VK_SUCCESS, DescriptorPoolCreationException(result));
}

DescriptorPoolVk::~DescriptorPoolVk() {
	vkDestroyDescriptorPool(
		Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		pool, nullptr);
}

VkDescriptorPool DescriptorPoolVk::GetPool() const {
	return pool;
}
