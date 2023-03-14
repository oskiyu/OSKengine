#pragma once

#include "IComputePipeline.h"
#include "IPipelineVk.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL ComputePipelineVk final : public IComputePipeline, public IPipelineVk {

	public:

		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}
