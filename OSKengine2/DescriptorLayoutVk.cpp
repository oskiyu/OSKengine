#include "DescriptorLayoutVk.h"

#include "MaterialLayoutSlot.h"
#include "ShaderBindingType.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "ShaderBindingTypeVk.h"
#include "DynamicArray.hpp"
#include "GpuVk.h"

#include "MaterialExceptions.h"
#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorLayoutVk::DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout)
	: slotLayout(slotLayout)  {

	DynamicArray<VkDescriptorSetLayoutBinding> bindings;
	DynamicArray<VkDescriptorBindingFlags> flags;

	for (auto& [name, binding] : slotLayout->bindings) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.glslIndex;
		layoutBinding.descriptorType = GetDescriptorTypeVk(binding.type);
		layoutBinding.descriptorCount = binding.numArrayLayers;
		layoutBinding.stageFlags = GetShaderStageVk(slotLayout->stage);
		layoutBinding.pImmutableSamplers = nullptr;

		bindings.Insert(layoutBinding);
		flags.Insert(VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo{};
	flagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	flagsCreateInfo.bindingCount = (uint32_t)bindings.GetSize();

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (uint32_t)bindings.GetSize();
	layoutInfo.pBindings = bindings.GetData();
	layoutInfo.pNext = nullptr;

	VkResult result = vkCreateDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&layoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, DescriptorLayoutCreationException(result));
}

DescriptorLayoutVk::~DescriptorLayoutVk() {
	vkDestroyDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		layout, nullptr);
}

VkDescriptorSetLayout DescriptorLayoutVk::GetLayout() const {
	return layout;
}

const MaterialLayoutSlot* DescriptorLayoutVk::GetMaterialSlotLayout() const {
	return slotLayout;
}
