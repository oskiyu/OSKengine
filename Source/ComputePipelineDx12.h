#pragma once

#include "IPipelineDx12.h"
#include "IComputePipeline.h"
#include "GpuDx12.h"

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <dxcapi.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL ComputePipelineDx12 : public IComputePipeline, public IPipelineDx12 {

	public:

		ComputePipelineDx12(const PipelineCreateInfo& info, const MaterialLayout* materialLayout, IGpu* device);

	private:

		ShaderStageDx12 computeShader{};

	};

}
