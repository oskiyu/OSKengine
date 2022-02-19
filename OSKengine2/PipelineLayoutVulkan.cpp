#include "PipelineLayoutVulkan.h"

#include <vulkan/vulkan.h>

#include "EnumFlags.hpp"
#include "MaterialLayout.h"
#include "ShaderBindingType.h"

#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "DescriptorLayoutVulkan.h"

using namespace OSK;


PipelineLayoutVulkan::PipelineLayoutVulkan(const MaterialLayout* materialLayout)
	: IPipelineLayout(materialLayout) {

	DynamicArray<VkDescriptorSetLayoutBinding> params;
	DynamicArray<OwnedPtr<DescriptorLayoutVulkan>> descLayouts;
	DynamicArray<VkDescriptorSetLayout> nativeDescLayouts;
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	for (auto& slotName : materialLayout->GetAllSlotNames()) {
		auto& slot = materialLayout->GetSlot(slotName);

		auto descLayout = new DescriptorLayoutVulkan(&slot);
		descLayouts.Insert(descLayout);
		nativeDescLayouts.Insert(descLayout->GetLayout());
	}

	// PIPELINE LAYOUT
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = nativeDescLayouts.GetSize();
	pipelineLayoutInfo.pSetLayouts = nativeDescLayouts.GetData();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el pipeline layout.");

	for (TSize i = 0; i < descLayouts.GetSize(); i++)
		delete descLayouts.At(i).GetPointer();
}

PipelineLayoutVulkan::~PipelineLayoutVulkan() {
	VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	vkDestroyPipelineLayout(device, layout, nullptr);
}

VkPipelineLayout PipelineLayoutVulkan::GetLayout() const {
	return layout;
}
