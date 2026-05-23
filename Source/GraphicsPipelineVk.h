#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGraphicsPipeline.h"
#include "IPipelineVk.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>


#include <optional>
#include "RenderpassVk.h"

namespace OSK::GRAPHICS {

	class GpuVk;

	class OSKAPI_CALL GraphicsPipelineVk final : public IGraphicsPipeline {

	public:

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) override;

		VkPipeline GetPipeline() const;

		void SetDebugName(const std::string& name) override;

	private:

		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadVertexShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadFragmentShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadTesselationControlShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadTesselationEvaluationShader(const std::string& path);

		IPipelineVk m_pipeline;

		IGpu* gpu = nullptr;

		std::optional<RenderpassVk> m_renderpass = std::nullopt;

	};

}

#endif
