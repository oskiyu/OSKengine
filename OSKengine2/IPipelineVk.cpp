#include "IPipelineVk.h"

#include "FileIO.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#include "ShaderBindingType.h"
#include "PipelinesExceptions.h"

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
