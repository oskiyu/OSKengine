#include "PipelineLayoutVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "EnumFlags.hpp"
#include "MaterialLayout.h"
#include "ShaderBindingType.h"

#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "DescriptorLayoutVk.h"
#include "ShaderBindingTypeVk.h"

#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

PipelineLayoutVk::PipelineLayoutVk(const MaterialLayout* materialLayout)
	: IPipelineLayout(materialLayout) {

	DynamicArray<VkDescriptorSetLayoutBinding> params;
	DynamicArray<OwnedPtr<DescriptorLayoutVk>> descLayouts;
	DynamicArray<VkDescriptorSetLayout> nativeDescLayouts;
	DynamicArray<VkPushConstantRange> pushConstantRanges;
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	DynamicArray<MaterialLayoutSlot> orderedSlots{};
	for (auto const& [name, slot] : materialLayout->GetAllSlots())
		orderedSlots.Insert(slot);

	if (!orderedSlots.IsEmpty()) {
		for (UIndex32 i = 0; i < orderedSlots.GetSize() - 1; i++) {
			for (UIndex32 j = 0; j < orderedSlots.GetSize() - 1; j++) {
				const auto& slot1 = orderedSlots[j + 1].glslSetIndex;
				const auto& slot2 = orderedSlots[j].glslSetIndex;

				if (slot1 < slot2) {
					MaterialLayoutSlot temp = orderedSlots[j + 1];
					orderedSlots[j + 1] = orderedSlots[j];
					orderedSlots[j] = temp;
				}
			}
		}
	}

	for (auto& slot : orderedSlots) {
		auto descLayout = new DescriptorLayoutVk(&slot, 0);
		descLayouts.Insert(descLayout);
		nativeDescLayouts.Insert(descLayout->GetLayout());
	}

	for (auto& i : materialLayout->GetAllPushConstants()) {
		VkPushConstantRange range{};

		range.stageFlags = GetShaderStageVk(i.second.stage);
		range.offset = i.second.offset;
		range.size = i.second.size;

		pushConstantRanges.Insert(range);
	}

	// PIPELINE LAYOUT
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(nativeDescLayouts.GetSize());
	pipelineLayoutInfo.pSetLayouts = nativeDescLayouts.GetData();
	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.GetSize());
	pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.GetData();
	
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, PipelineLayoutCreationException(result));

	for (UIndex32 i = 0; i < descLayouts.GetSize(); i++)
		delete descLayouts.At(i).GetPointer();
}

PipelineLayoutVk::~PipelineLayoutVk() {
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	vkDestroyPipelineLayout(device, layout, nullptr);
}

VkPipelineLayout PipelineLayoutVk::GetLayout() const {
	return layout;
}

#endif
