#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

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

#endif
