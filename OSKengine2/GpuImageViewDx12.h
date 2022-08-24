#pragma once

#include "IGpuImageView.h"
#include "DescriptorDx12.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuImageViewDx12 : public IGpuImageView {

	public:

		GpuImageViewDx12(DescriptorDx12 descriptor, SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuAddress() const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuAddress() const;
		ID3D12DescriptorHeap* GetDescriptorHeap() const;

	private:

		DescriptorDx12 descriptor{};

	};

}
