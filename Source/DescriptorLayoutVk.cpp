#include "DescriptorLayoutVk.h"

#include "DynamicArray.hpp"
#include "GpuVk.h"
#include "IRenderer.h"
#include "MaterialLayoutSlot.h"
#include "OSKengine.h"
#include "ShaderBindingType.h"
#include "ShaderBindingTypeVk.h"

#include "MaterialExceptions.h"
#include <vulkan/vulkan.h>

#include "BindlessLimits.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorLayoutVk::DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout, USize32 maxSets)
	: m_slotLayout(slotLayout)  {

	const bool isBindless = Engine::GetRenderer()->GetGpu()->SupportsBindlessResources();

	DynamicArray<VkDescriptorSetLayoutBinding> bindings;
	DynamicArray<VkDescriptorBindingFlags> flags;

	for (const auto& [name, binding] : slotLayout->bindings) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.glslIndex;
		layoutBinding.descriptorType = GetDescriptorTypeVk(binding.type);
		layoutBinding.descriptorCount = binding.numArrayLayers == 0
			? MAX_BINDLESS_RESOURCES
			: binding.numArrayLayers;
		layoutBinding.stageFlags = GetShaderStageVk(slotLayout->stage);
		layoutBinding.pImmutableSamplers = nullptr;

		bindings.Insert(layoutBinding);
		flags.Insert(
			VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | 
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | 
			VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo{};
	flagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	flagsCreateInfo.bindingCount = (uint32_t)bindings.GetSize();
	flagsCreateInfo.pBindingFlags = flags.GetData();
	flagsCreateInfo.pNext = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.GetSize());
	layoutInfo.pBindings = bindings.GetData();
	layoutInfo.pNext = nullptr;

	if (isBindless) {
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
		layoutInfo.pNext = &flagsCreateInfo;
	}

	VkResult result = vkCreateDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&layoutInfo, nullptr, &m_layoutVk);
	OSK_ASSERT(result == VK_SUCCESS, DescriptorLayoutCreationException(result));
}

DescriptorLayoutVk::~DescriptorLayoutVk() {
	vkDestroyDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		m_layoutVk, nullptr);
}

VkDescriptorSetLayout DescriptorLayoutVk::GetLayout() const {
	return m_layoutVk;
}

const MaterialLayoutSlot* DescriptorLayoutVk::GetMaterialSlotLayout() const {
	return m_slotLayout;
}
