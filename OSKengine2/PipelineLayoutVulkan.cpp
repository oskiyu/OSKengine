#include "PipelineLayoutVulkan.h"

#include <vulkan/vulkan.h>

#include "EnumFlags.hpp"
#include "MaterialLayout.h"
#include "ShaderBindingType.h"

#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"

using namespace OSK;

VkDescriptorType GetDescriptorTypeVk(ShaderBindingType type) {
	switch (type) {
	case OSK::ShaderBindingType::UNIFORM_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case OSK::ShaderBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	default:
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkShaderStageFlags GetShaderStageVk(ShaderStage stage) {
	VkShaderStageFlags output = 0;

	if (EFTraits::HasFlag(stage, ShaderStage::VERTEX))
		output |= VK_SHADER_STAGE_VERTEX_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		output |= VK_SHADER_STAGE_FRAGMENT_BIT;

	return output;
}


PipelineLayoutVulkan::PipelineLayoutVulkan(const MaterialLayout* materialLayout)
	: IPipelineLayout(materialLayout) {

	DynamicArray<VkDescriptorSetLayoutBinding> params;
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	for (auto& slotName : materialLayout->GetAllSlotNames()) {
		auto& slot = materialLayout->GetSlot(slotName);

		for (auto& set : slot.bindings) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = set.second.glslIndex;
			binding.descriptorCount = 1;
			binding.descriptorType = GetDescriptorTypeVk(set.second.type);
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = GetShaderStageVk(slot.stage);

			params.Insert(binding);
		}

		// DESCRPITOR LAYOUT
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = params.GetSize();
		layoutInfo.pBindings = params.GetData();

		VkDescriptorSetLayout layout{};
		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);
		OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el descriptor layout.");

		descLayouts.Insert(layout);
	}

	// PIPELINE LAYOUT
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = descLayouts.GetSize();
	pipelineLayoutInfo.pSetLayouts = descLayouts.GetData();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el pipeline layout.");
}

PipelineLayoutVulkan::~PipelineLayoutVulkan() {
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	for (TSize i = 0; i < descLayouts.GetSize(); i++)
		vkDestroyDescriptorSetLayout(device, descLayouts.At(i), nullptr);

	vkDestroyPipelineLayout(device, layout, nullptr);
}

VkPipelineLayout PipelineLayoutVulkan::GetLayout() const {
	return layout;
}
