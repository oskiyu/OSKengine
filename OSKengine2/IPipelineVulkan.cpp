#include "IPipelineVulkan.h"

#include "FileIO.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVulkan.h"

#include "ShaderBindingType.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IPipelineVulkan::~IPipelineVulkan() {
	for (TSize i = 0; i < shaderModulesToDelete.GetSize(); i++)
		vkDestroyShaderModule(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			shaderModulesToDelete.At(i), nullptr);

	vkDestroyPipeline(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		pipeline, nullptr);
}

VkPipeline IPipelineVulkan::GetPipeline() const {
	return pipeline;
}

VkVertexInputBindingDescription IPipelineVulkan::GetBindingDescription(const VertexInfo& info) const {
	VkVertexInputBindingDescription bindingDescription{};

	TSize size = 0;
	for (const auto& i : info.entries)
		size += i.size;

	bindingDescription.binding = 0;
	bindingDescription.stride = size;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

VkFormat IPipelineVulkan::GetVertexAttribFormat(const VertexInfo::Entry& entry) const {
	switch (entry.type) {

	case VertexInfo::Entry::Type::INT:
		if (entry.size == 2 * sizeof(int)) return VK_FORMAT_R32G32_SINT;
		if (entry.size == 3 * sizeof(int)) return VK_FORMAT_R32G32B32_SINT;
		if (entry.size == 4 * sizeof(int)) return VK_FORMAT_R32G32B32A32_SINT;

	case VertexInfo::Entry::Type::FLOAT:
		if (entry.size == 2 * sizeof(float)) return VK_FORMAT_R32G32_SFLOAT;
		if (entry.size == 3 * sizeof(float)) return VK_FORMAT_R32G32B32_SFLOAT;
		if (entry.size == 4 * sizeof(float)) return VK_FORMAT_R32G32B32A32_SFLOAT;
	}

	OSK_ASSERT(false, "Formato de v�rtice incorecto.");
	return VK_FORMAT_MAX_ENUM;
}

DynamicArray<VkVertexInputAttributeDescription> IPipelineVulkan::GetAttributeDescription(const VertexInfo& info) const {
	DynamicArray<VkVertexInputAttributeDescription> output;

	TSize offset = 0;
	for (TSize i = 0; i < info.entries.GetSize(); i++) {
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

VkPipelineColorBlendAttachmentState IPipelineVulkan::GetColorBlendInfo(const PipelineCreateInfo& info) const {
	VkPipelineColorBlendAttachmentState output{};

	output.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	output.blendEnable = VK_TRUE;
	output.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	output.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	output.colorBlendOp = VK_BLEND_OP_ADD;
	output.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	output.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	output.alphaBlendOp = VK_BLEND_OP_ADD;

	return output;
}

VkPolygonMode IPipelineVulkan::GetPolygonMode(PolygonMode mode) const {
	switch (mode) {
	case PolygonMode::FILL:
		return VK_POLYGON_MODE_FILL;
	case PolygonMode::LINE:
		return VK_POLYGON_MODE_LINE;
	default:
		return VK_POLYGON_MODE_FILL;
	}
}

VkCullModeFlagBits IPipelineVulkan::GetCullMode(PolygonCullMode mode) const {
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

VkFrontFace IPipelineVulkan::GetFrontFaceMode(PolygonFrontFaceType type) const {
	switch (type) {
	case PolygonFrontFaceType::CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
	case PolygonFrontFaceType::COUNTERCLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		return VK_FRONT_FACE_CLOCKWISE;
	}
}

VkShaderStageFlagBits IPipelineVulkan::GetShaderStageVk(ShaderStage stage) const {
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
	
	OSK_ASSERT(false, "No se reconoce el shader stage " + ToString<ShaderStage>(stage));
	return VK_SHADER_STAGE_ALL;
}

ShaderStageVulkan IPipelineVulkan::LoadShader(const std::string& path, ShaderStage stage) {
	ShaderStageVulkan output{};

	// Lee el c�digo SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.GetData());

	VkResult result = vkCreateShaderModule(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		&createInfo, nullptr, &output.shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido cargar el tesselation control shader.");

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	output.shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	output.shaderCreateInfo.stage = GetShaderStageVk(stage);
	output.shaderCreateInfo.module = output.shaderModule;
	output.shaderCreateInfo.pName = "main";

	shaderStagesInfo.Insert(output.shaderCreateInfo);
	shaderModulesToDelete.Insert(output.shaderModule);

	return output;
}
