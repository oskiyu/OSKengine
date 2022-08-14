#pragma once

#include "IGraphicsPipeline.h"
#include "IPipelineVulkan.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuVulkan;

	class OSKAPI_CALL GraphicsPipelineVulkan : public IGraphicsPipeline, public IPipelineVulkan {

	public:

		GraphicsPipelineVulkan();

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		void SetDebugName(const std::string& name) override;

	private:

		VkPipelineRasterizationStateCreateInfo GetResterizerInfo(const PipelineCreateInfo& info) const;
		VkPipelineDepthStencilStateCreateInfo GetDepthInfo(const PipelineCreateInfo& info) const;
		VkPipelineMultisampleStateCreateInfo GetMsaaInfo(const PipelineCreateInfo& info, const GpuVulkan& gpu) const;
		VkPipelineTessellationStateCreateInfo GetTesselationInfo(const PipelineCreateInfo& info) const;

		void LoadVertexShader(const std::string& path);
		void LoadFragmentShader(const std::string& path);
		void LoadTesselationControlShader(const std::string& path);
		void LoadTesselationEvaluationShader(const std::string& path);

		IGpu* gpu = nullptr;

	};

}
