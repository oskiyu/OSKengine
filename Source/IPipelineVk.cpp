#include "IPipelineVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "FileIO.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include "ShaderBindingType.h"
#include "PipelinesExceptions.h"

#include "RendererVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IPipelineVk::~IPipelineVk() {
	for (UIndex32 i = 0; i < shaderModulesToDelete.GetSize(); i++)
		vkDestroyShaderModule(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
			shaderModulesToDelete.At(i), nullptr);

	vkDestroyPipeline(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		pipeline, nullptr);
}

VkPipeline IPipelineVk::GetPipeline() const {
	return pipeline;
}

VkVertexInputBindingDescription IPipelineVk::GetBindingDescription(const VertexInfo& info) const {
	VkVertexInputBindingDescription bindingDescription{};

	USize32 size = 0;
	for (const auto& i : info.entries)
		size += i.size;

	bindingDescription.binding = 0;
	bindingDescription.stride = size;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

VkShaderModule IPipelineVk::CreateShaderModule(std::span<const char> code, std::string_view name, VkDevice logicalDevice) {
	VkShaderModule output = VK_NULL_HANDLE;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkResult result = vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &output);
	OSK_ASSERT(result == VK_SUCCESS, ShaderLoadingException(result));

	if (RendererVk::pvkSetDebugUtilsObjectNameEXT) {
		VkDebugUtilsObjectNameInfoEXT nameInfo{};
		nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.objectType = VK_OBJECT_TYPE_SHADER_MODULE;
		nameInfo.objectHandle = (uint64_t)output;
		nameInfo.pObjectName = name.data();

		RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}

	return output;
}

VkFormat IPipelineVk::GetVertexAttribFormat(const VertexInfo::Entry& entry) const {
	switch (entry.type) {

	case VertexInfo::Entry::Type::INT:
		if (entry.size == 2 * sizeof(int)) return VK_FORMAT_R32G32_SINT;
		if (entry.size == 3 * sizeof(int)) return VK_FORMAT_R32G32B32_SINT;
		if (entry.size == 4 * sizeof(int)) return VK_FORMAT_R32G32B32A32_SINT;
		break;

	case VertexInfo::Entry::Type::FLOAT:
		if (entry.size == 2 * sizeof(float)) return VK_FORMAT_R32G32_SFLOAT;
		if (entry.size == 3 * sizeof(float)) return VK_FORMAT_R32G32B32_SFLOAT;
		if (entry.size == 4 * sizeof(float)) return VK_FORMAT_R32G32B32A32_SFLOAT;
		break;

	case VertexInfo::Entry::Type::UNSIGNED_INT:
		if (entry.size == 1 * sizeof(unsigned int)) return VK_FORMAT_R32_UINT;
		if (entry.size == 2 * sizeof(unsigned int)) return VK_FORMAT_R32G32_UINT;
		if (entry.size == 3 * sizeof(unsigned int)) return VK_FORMAT_R32G32B32_UINT;
		if (entry.size == 4 * sizeof(unsigned int)) return VK_FORMAT_R32G32B32A32_UINT;
		break;
	}

	OSK_ASSERT(false, NotImplementedException());
	return VK_FORMAT_MAX_ENUM;
}

DynamicArray<VkVertexInputAttributeDescription> IPipelineVk::GetAttributeDescription(const VertexInfo& info) const {
	DynamicArray<VkVertexInputAttributeDescription> output;

	USize32 offset = 0;
	for (UIndex32 i = 0; i < info.entries.GetSize(); i++) {
		VkVertexInputAttributeDescription desc{};

		desc.binding = 0;
		desc.location = i;
		desc.offset = offset;
		desc.format = GetVertexAttribFormat(info.entries.At(i));

		output.Insert(desc);

		offset += info.entries.At(i).size;
	}

	return output;
}

VkPipelineColorBlendAttachmentState IPipelineVk::GetColorBlendInfo(const PipelineCreateInfo& info) const {
	VkPipelineColorBlendAttachmentState output{};

	output.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	output.blendEnable = info.useAlphaBlending ? VK_TRUE : VK_FALSE;

	output.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	output.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	output.colorBlendOp = VK_BLEND_OP_ADD;

	output.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	output.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	output.alphaBlendOp = VK_BLEND_OP_ADD;

	return output;
}

VkPolygonMode IPipelineVk::GetPolygonMode(PolygonMode mode) const {
	switch (mode) {
	case PolygonMode::TRIANGLE_FILL:
		return VK_POLYGON_MODE_FILL;
	case PolygonMode::TRIANGLE_WIDEFRAME:
	case PolygonMode::LINE:
		return VK_POLYGON_MODE_LINE;
	default:
		return VK_POLYGON_MODE_FILL;
	}
}

VkCullModeFlagBits IPipelineVk::GetCullMode(PolygonCullMode mode) const {
	switch (mode) {
	case PolygonCullMode::FRONT:
		return VK_CULL_MODE_FRONT_BIT;
	case PolygonCullMode::BACK:
		return VK_CULL_MODE_BACK_BIT;
	case PolygonCullMode::NONE:
		return VK_CULL_MODE_NONE;
	default:
		return VK_CULL_MODE_FRONT_AND_BACK;
	}
}

VkFrontFace IPipelineVk::GetFrontFaceMode(PolygonFrontFaceType type) const {
	switch (type) {
	case PolygonFrontFaceType::CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
	case PolygonFrontFaceType::COUNTERCLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		return VK_FRONT_FACE_CLOCKWISE;
	}
}

VkShaderStageFlagBits IPipelineVk::GetShaderStageVk(ShaderStage stage) const {
	if (EFTraits::HasFlag(stage, ShaderStage::VERTEX))
		return VK_SHADER_STAGE_VERTEX_BIT;
	if (EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	if (EFTraits::HasFlag(stage, ShaderStage::TESSELATION_CONTROL))
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if (EFTraits::HasFlag(stage, ShaderStage::TESSELATION_EVALUATION))
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::RT_RAYGEN))
		return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	if (EFTraits::HasFlag(stage, ShaderStage::RT_CLOSEST_HIT))
		return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	if (EFTraits::HasFlag(stage, ShaderStage::RT_MISS))
		return VK_SHADER_STAGE_MISS_BIT_KHR;

	if (EFTraits::HasFlag(stage, ShaderStage::COMPUTE))
		return VK_SHADER_STAGE_COMPUTE_BIT;
	
	OSK_ASSERT(false, NotImplementedException());
	return VK_SHADER_STAGE_ALL;
}

ShaderStageVk IPipelineVk::LoadShader(const std::string& path, ShaderStage stage) {
	ShaderStageVk output{};

	// Lee el código SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.GetData());

	VkResult result = vkCreateShaderModule(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		&createInfo, nullptr, &output.shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, ShaderLoadingException(result));

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	output.shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	output.shaderCreateInfo.stage = GetShaderStageVk(stage);
	output.shaderCreateInfo.module = output.shaderModule;
	output.shaderCreateInfo.pName = "main";

	shaderStagesInfo.Insert(output.shaderCreateInfo);
	shaderModulesToDelete.Insert(output.shaderModule);

	return output;
}


VkPipelineRasterizationStateCreateInfo IPipelineVk::GetResterizerInfo(const PipelineCreateInfo& info) const {
	VkPipelineRasterizationStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	output.depthClampEnable = VK_FALSE; // FALSE: si el objeto está fuera de los límites no se renderiza.
	output.rasterizerDiscardEnable = VK_FALSE;
	//Modo de polígonos:
	//	VK_POLYGON_MODE_FILL
	//	VK_POLYGON_MODE_LINE
	//	VK_POLYGON_MODE_POINT: vértice -> punto.
	output.polygonMode = GetPolygonMode(info.polygonMode);
	output.lineWidth = 1.0f;
	output.cullMode = GetCullMode(info.cullMode);
	output.frontFace = GetFrontFaceMode(info.frontFaceType);
	output.depthBiasEnable = VK_FALSE;
	output.depthBiasConstantFactor = 0.0f;
	output.depthBiasClamp = 0.0f;
	output.depthBiasSlopeFactor = 0.0f;

	return output;
}

VkPipelineDepthStencilStateCreateInfo IPipelineVk::GetDepthInfo(const PipelineCreateInfo& info) const {
	VkPipelineDepthStencilStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	output.depthTestEnable = info.depthTestingType != DepthTestingType::NONE;
	output.depthWriteEnable = info.depthTestingType == DepthTestingType::READ_WRITE;
	output.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
	output.depthBoundsTestEnable = VK_FALSE;
	output.minDepthBounds = 1.0f;
	output.maxDepthBounds = 0.0f;
	output.stencilTestEnable = VK_FALSE;
	output.front = {};
	output.back = {};

	return output;
}

VkPipelineMultisampleStateCreateInfo IPipelineVk::GetMsaaInfo(const PipelineCreateInfo& info, const GpuVk& gpu) const {
	VkPipelineMultisampleStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	output.sampleShadingEnable = VK_FALSE;
	output.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//(VkSampleCountFlagBits)gpu.GetInfo().maxMsaaSamples;
	output.minSampleShading = 1.0f;
	output.pSampleMask = nullptr;
	output.alphaToCoverageEnable = VK_FALSE;
	output.alphaToOneEnable = VK_FALSE;

	return output;
}

VkPipelineTessellationStateCreateInfo IPipelineVk::GetTesselationInfo(const PipelineCreateInfo& info) const {
	VkPipelineTessellationStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	output.pNext = NULL;
	output.flags = 0;
	output.patchControlPoints = 3;

	return output;
}

#endif
