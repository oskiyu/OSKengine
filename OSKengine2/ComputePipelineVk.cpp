#include "ComputePipelineVk.h"

#include "PipelineLayoutVk.h"
#include "MaterialLayout.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ComputePipelineVk::Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info) {
	layout = new PipelineLayoutVk(&materialLayout);

	const ShaderStageVk stage = LoadShader(info.computeShaderPath, ShaderStage::COMPUTE);
	
	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.stage = stage.shaderCreateInfo;
	createInfo.layout = layout->As<PipelineLayoutVk>()->GetLayout();
	createInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	VkResult result = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, PipelineCreationException(result));
}
