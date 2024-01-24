#pragma once

#include "IGpuImageView.h"
#include "DescriptorDx12.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuImageViewDx12 : public IGpuImageView {

	public:

		GpuImageViewDx12(const GpuImage* img, DescriptorDx12 descriptor, const GpuImageViewConfig& config);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuAddress() const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuAddress() const;
		ID3D12DescriptorHeap* GetDescriptorHeap() const;

	private:

		DescriptorDx12 descriptor{};

	};

}
