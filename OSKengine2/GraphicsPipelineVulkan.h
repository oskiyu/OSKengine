#pragma once

#include "IGraphicsPipeline.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class RenderpassVulkan;

	class OSKAPI_CALL GraphicsPipelineVulkan : public IGraphicsPipeline {

	public:

		GraphicsPipelineVulkan(RenderpassVulkan* renderpass);
		~GraphicsPipelineVulkan();

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info) override;

		VkPipeline GetPipeline() const;

	private:

		void LoadVertexShader(const std::string& path);
		void LoadFragmentShader(const std::string& path);

		DynamicArray<VkPipelineShaderStageCreateInfo> shaderStagesInfo;
		DynamicArray<VkShaderModule> shaderModulesToDelete;

		VkPipeline pipeline = 0;

		RenderpassVulkan* targetRenderpass = nullptr;

		IGpu* gpu = nullptr;

	};

}
