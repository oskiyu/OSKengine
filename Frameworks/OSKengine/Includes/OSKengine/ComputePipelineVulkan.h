#pragma once

#include "IComputePipeline.h"
#include "IPipelineVulkan.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL ComputePipelineVulkan : public IComputePipeline, public IPipelineVulkan {

	public:

		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}
