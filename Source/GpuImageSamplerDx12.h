#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IGpuImageSampler.h"
#include <d3d12.h>

namespace OSK::GRAPHICS {

	class GpuDx12;

	class OSKAPI_CALL GpuImageSamplerDx12 final : public IGpuImageSampler {

	public:

		explicit GpuImageSamplerDx12(
			const GpuImageSamplerDesc& info,
			const GpuDx12* gpu);
		~GpuImageSamplerDx12() override;

	private:

		D3D12_CPU_DESCRIPTOR_HANDLE m_handle{};

	};

}

#endif
