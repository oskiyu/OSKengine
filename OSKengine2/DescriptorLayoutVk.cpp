#include "DescriptorLayoutVk.h"

#include "MaterialLayoutSlot.h"
#include "ShaderBindingType.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "ShaderBindingTypeVk.h"
#include "DynamicArray.hpp"
#include "GpuVk.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorLayoutVk::DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout)
	: slotLayout(slotLayout)  {

	DynamicArray<VkDescriptorSetLayoutBinding> bindings;

	for (auto& [name, binding] : slotLayout->bindings) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.glslIndex;
		layoutBinding.descriptorType = GetDescriptorTypeVk(binding.type);
		layoutBinding.descriptorCount = binding.numArrayLayers;
		layoutBinding.stageFlags = GetShaderStageVk(slotLayout->stage);
		layoutBinding.pImmutableSamplers = nullptr;

		bindings.Insert(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (uint32_t)bindings.GetSize();
	layoutInfo.pBindings = bindings.GetData();

	VkResult result = vkCreateDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), 
		&layoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el descriptor set layout.");
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
