#pragma once

#include "IRaytracingPipeline.h"
#include "IPipelineVk.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class MaterialLayout;

	class OSKAPI_CALL RaytracingPipelineVk final : public IRaytracingPipeline, public IPipelineVk {

	public:

		RaytracingPipelineVk() = default;
		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}
