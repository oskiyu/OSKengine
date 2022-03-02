#pragma once

#include "IGpuImage.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK {

	class OSKAPI_CALL GpuImageDx12 : public GpuImage {

	public:

		GpuImageDx12(unsigned int sizeX, unsigned int sizeY, Format format);

		void SetResource(const ComPtr<ID3D12Resource>& resource);
		void _SetDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap);
		ID3D12Resource* GetResource() const;
		ID3D12DescriptorHeap* GetDescriptorHeap() const;

	private:

		ComPtr<ID3D12Resource> resource;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	};

}
