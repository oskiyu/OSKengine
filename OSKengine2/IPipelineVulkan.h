#pragma once

#include <vulkan/vulkan.h>

#include "VertexInfo.h"
#include "PipelineCreateInfo.h"

namespace OSK::GRAPHICS {

	enum class ShaderStage;

	struct ShaderStageVulkan {
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo shaderCreateInfo{};
	};

	class OSKAPI_CALL IPipelineVulkan {

	public:

		virtual ~IPipelineVulkan();
		VkPipeline GetPipeline() const;

	protected:

		DynamicArray<VkPipelineShaderStageCreateInfo> shaderStagesInfo;
		DynamicArray<VkShaderModule> shaderModulesToDelete;

		VkPipeline pipeline = VK_NULL_HANDLE;

		VkVertexInputBindingDescription GetBindingDescription(const VertexInfo& info) const;
		VkFormat GetVertexAttribFormat(const VertexInfo::Entry& entry) const;
		DynamicArray<VkVertexInputAttributeDescription> GetAttributeDescription(const VertexInfo& info) const;
		VkPipelineColorBlendAttachmentState GetColorBlendInfo(const PipelineCreateInfo& info) const;
		VkPolygonMode GetPolygonMode(PolygonMode mode) const;
		VkCullModeFlagBits GetCullMode(PolygonCullMode mode) const;
		VkFrontFace GetFrontFaceMode(PolygonFrontFaceType type) const;
		VkShaderStageFlagBits GetShaderStageVk(ShaderStage stage) const;

		ShaderStageVulkan LoadShader(const std::string& path, ShaderStage stage);

	};

}
