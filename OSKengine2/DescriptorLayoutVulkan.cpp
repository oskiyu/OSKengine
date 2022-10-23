#include "DescriptorLayoutVulkan.h"

#include "MaterialLayoutSlot.h"
#include "ShaderBindingType.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "ShaderBindingTypeVulkan.h"
#include "DynamicArray.hpp"
#include "GpuVulkan.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorLayoutVulkan::DescriptorLayoutVulkan(const MaterialLayoutSlot* slotLayout)
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

	VkResult result = vkCreateDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), 
		&layoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el descriptor set layout.");
}

DescriptorLayoutVulkan::~DescriptorLayoutVulkan() {
	vkDestroyDescriptorSetLayout(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		layout, nullptr);
}

VkDescriptorSetLayout DescriptorLayoutVulkan::GetLayout() const {
	return layout;
}

const MaterialLayoutSlot* DescriptorLayoutVulkan::GetMaterialSlotLayout() const {
	return slotLayout;
}
