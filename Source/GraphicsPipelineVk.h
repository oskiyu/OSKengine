#pragma once

#include "IGraphicsPipeline.h"
#include "IPipelineVk.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuVk;

	class OSKAPI_CALL GraphicsPipelineVk final : public IGraphicsPipeline, public IPipelineVk {

	public:

		GraphicsPipelineVk();

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		void SetDebugName(const std::string& name) override;

	private:

		VkPipelineRasterizationStateCreateInfo GetResterizerInfo(const PipelineCreateInfo& info) const;
		VkPipelineDepthStencilStateCreateInfo GetDepthInfo(const PipelineCreateInfo& info) const;
		VkPipelineMultisampleStateCreateInfo GetMsaaInfo(const PipelineCreateInfo& info, const GpuVk& gpu) const;
		VkPipelineTessellationStateCreateInfo GetTesselationInfo(const PipelineCreateInfo& info) const;

		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadVertexShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadFragmentShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadTesselationControlShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadTesselationEvaluationShader(const std::string& path);

		IGpu* gpu = nullptr;

	};

}
