#pragma once

#include "IComputePipeline.h"
#include "IPipelineVk.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL ComputePipelineVk final : public IComputePipeline, public IPipelineVk {

	public:

		/// @throws PipelineCreationException Si hay un error con Vulkan.
		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}
