#include "ComputePipelineVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "PipelineLayoutVk.h"
#include "MaterialLayout.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

template <VulkanTarget Target>
void ComputePipelineVk<Target>::Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info) {
	layout = MakeUnique<PipelineLayoutVk<Target>>(&materialLayout);

	const ShaderStageVk stage = m_pipeline.LoadShader(info.computeShaderPath, ShaderStage::COMPUTE);
	
	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.stage = stage.shaderCreateInfo;
	createInfo.layout = layout->As<PipelineLayoutVk<Target>>()->GetLayout();
	createInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();
	VkResult result = vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline.pipeline);
	OSK_ASSERT(result == VK_SUCCESS, PipelineCreationException(result));
}

template <VulkanTarget Target>
VkPipeline ComputePipelineVk<Target>::GetPipeline() const {
	return m_pipeline.pipeline;
}

#endif
