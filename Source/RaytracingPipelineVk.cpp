#include "RaytracingPipelineVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "MaterialLayout.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "PipelineLayoutVk.h"
#include "RtShaderTableVk.h"
#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RaytracingPipelineVk::Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info) {
	layout = MakeUnique<PipelineLayoutVk>(&materialLayout);

	DynamicArray<VkRayTracingShaderGroupCreateInfoKHR> shaderGroupCreateInfos;

	VkSpecializationMapEntry specializationMapEntry{};
	specializationMapEntry.constantID = 0;
	specializationMapEntry.offset = 0;
	specializationMapEntry.size = sizeof(USize32);

	const USize32 maxRecursion = 1; ///@todo Permitir su modificación.

	VkSpecializationInfo specializationInfo{};
	specializationInfo.dataSize = sizeof(USize32);
	specializationInfo.pData = &maxRecursion;
	specializationInfo.pMapEntries = &specializationMapEntry;
	specializationInfo.mapEntryCount = 1;

	// Raygen
	LoadShader(info.rtRaygenShaderPath, ShaderStage::RT_RAYGEN);

	VkRayTracingShaderGroupCreateInfoKHR raygenShaderGroup{};
	raygenShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	raygenShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	raygenShaderGroup.generalShader = static_cast<uint32_t>(shaderStagesInfo.GetSize() - 1);
	raygenShaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
	raygenShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
	raygenShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	shaderGroupCreateInfos.Insert(raygenShaderGroup);

	shaderStagesInfo.At(0).pSpecializationInfo = &specializationInfo;

	// Miss
	LoadShader(info.rtMissShaderPath, ShaderStage::RT_MISS);

	VkRayTracingShaderGroupCreateInfoKHR missShaderGroup{};
	missShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	missShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	missShaderGroup.generalShader = static_cast<uint32_t>(shaderStagesInfo.GetSize() - 1);
	missShaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
	missShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
	missShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	shaderGroupCreateInfos.Insert(missShaderGroup);

	// Hit
	LoadShader(info.rtClosestHitShaderPath, ShaderStage::RT_CLOSEST_HIT);

	VkRayTracingShaderGroupCreateInfoKHR closestHitShaderGroup{};
	closestHitShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	closestHitShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	closestHitShaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
	closestHitShaderGroup.closestHitShader = static_cast<uint32_t>(shaderStagesInfo.GetSize() - 1);
	closestHitShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
	closestHitShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	shaderGroupCreateInfos.Insert(closestHitShaderGroup);

	VkRayTracingPipelineCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	createInfo.stageCount = static_cast<uint32_t>(shaderStagesInfo.GetSize());
	createInfo.pStages = shaderStagesInfo.GetData();
	createInfo.groupCount = static_cast<uint32_t>(shaderGroupCreateInfos.GetSize());
	createInfo.pGroups = shaderGroupCreateInfos.GetData();
	createInfo.maxPipelineRayRecursionDepth = 1; // @todo config
	createInfo.layout = layout->As<PipelineLayoutVk>()->GetLayout();

	VkResult result = RendererVk::pvkCreateRayTracingPipelinesKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, PipelineCreationException(result));

	shaderTable = MakeUnique<RtShaderTableVk>(static_cast<USize32>(shaderGroupCreateInfos.GetSize()), pipeline);
}

#endif
