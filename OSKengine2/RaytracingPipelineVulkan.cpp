#include "RaytracingPipelineVulkan.h"

#include "MaterialLayout.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "PipelineLayoutVulkan.h"
#include "RtShaderTableVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RaytracingPipelineVulkan::Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info) {
	layout = new PipelineLayoutVulkan(&materialLayout);

	DynamicArray<VkRayTracingShaderGroupCreateInfoKHR> shaderGroupCreateInfos;

	VkSpecializationMapEntry specializationMapEntry{};
	specializationMapEntry.constantID = 0;
	specializationMapEntry.offset = 0;
	specializationMapEntry.size = sizeof(TSize);

	const TSize maxRecursion = 4;

	VkSpecializationInfo specializationInfo{};
	specializationInfo.dataSize = sizeof(TSize);
	specializationInfo.pData = &maxRecursion;
	specializationInfo.pMapEntries = &specializationMapEntry;
	specializationInfo.mapEntryCount = 1;

	// Raygen
	LoadShader(info.rtRaygenShaderPath, ShaderStage::RT_RAYGEN);

	VkRayTracingShaderGroupCreateInfoKHR raygenShaderGroup{};
	raygenShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	raygenShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	raygenShaderGroup.generalShader = shaderStagesInfo.GetSize() - 1;
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
	missShaderGroup.generalShader = shaderStagesInfo.GetSize() - 1;
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
	closestHitShaderGroup.closestHitShader = shaderStagesInfo.GetSize() - 1;
	closestHitShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
	closestHitShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	shaderGroupCreateInfos.Insert(closestHitShaderGroup);

	VkRayTracingPipelineCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	createInfo.stageCount = shaderStagesInfo.GetSize();
	createInfo.pStages = shaderStagesInfo.GetData();
	createInfo.groupCount = shaderGroupCreateInfos.GetSize();
	createInfo.pGroups = shaderGroupCreateInfos.GetData();
	createInfo.maxPipelineRayRecursionDepth = 8; // TODO: config
	createInfo.layout = layout->As<PipelineLayoutVulkan>()->GetLayout();

	VkResult result = RendererVulkan::pvkCreateRayTracingPipelinesKHR(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el pipeline de trazado de rayos. Code: " + std::to_string(result));

	shaderTable = new RtShaderTableVulkan(shaderGroupCreateInfos.GetSize(), pipeline);
}
