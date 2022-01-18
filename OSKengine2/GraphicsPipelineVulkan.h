#pragma once

#include "IGraphicsPipeline.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK {

	class RenderpassVulkan;

	class OSKAPI_CALL GraphicsPipelineVulkan : public IGraphicsPipeline {

	public:

		GraphicsPipelineVulkan(RenderpassVulkan* renderpass);
		~GraphicsPipelineVulkan();

		void Create(IGpu* device, const PipelineCreateInfo& info) override;

		VkPipeline GetPipeline() const;
		VkPipelineLayout GetLayout() const;

	private:

		void LoadVertexShader(const std::string& path);
		void LoadFragmentShader(const std::string& path);

		DynamicArray<VkPipelineShaderStageCreateInfo> shaderStagesInfo;
		DynamicArray<VkShaderModule> shaderModulesToDelete;

		void SetLayout(IGpu* device);

		VkPipeline pipeline = 0;
		VkPipelineLayout layout = 0;

		RenderpassVulkan* targetRenderpass = nullptr;

		IGpu* gpu = nullptr;

	};

}
