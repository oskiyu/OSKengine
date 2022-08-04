#include "ComputePipelineVulkan.h"

#include "PipelineLayoutVulkan.h"
#include "MaterialLayout.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ComputePipelineVulkan::Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info) {
	layout = new PipelineLayoutVulkan(&materialLayout);

	const ShaderStageVulkan stage = LoadShader(info.computeShaderPath, ShaderStage::COMPUTE);
	
	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.stage = stage.shaderCreateInfo;
	createInfo.layout = layout->As<PipelineLayoutVulkan>()->GetLayout();
	createInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear el compute pipeline.");
}
