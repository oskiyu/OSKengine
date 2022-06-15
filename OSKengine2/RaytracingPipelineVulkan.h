#pragma once

#include "IRaytracingPipeline.h"
#include "IPipelineVulkan.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class MaterialLayout;

	class OSKAPI_CALL RaytracingPipelineVulkan : public IRaytracingPipeline, public IPipelineVulkan {

	public:

		RaytracingPipelineVulkan() = default;
		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}
