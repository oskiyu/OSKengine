#pragma once

#include "IGpuImage.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuImageDx12 : public GpuImage {

	public:

		GpuImageDx12(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc);

		void SetResource(const ComPtr<ID3D12Resource>& resource);

		ID3D12Resource* GetResource() const;

		void SetDebugName(const std::string& name) override;

	protected:

		OwnedPtr<IGpuImageView> CreateView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const override;

	private:

		ComPtr<ID3D12Resource> resource;

		ComPtr<ID3D12DescriptorHeap> arraySampledDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> arrayColorDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> arrayDepthStencilDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> arrayDepthDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> arrayStencilDescriptorHeap;

		ComPtr<ID3D12DescriptorHeap> sampledDescriptorsHeap;
		ComPtr<ID3D12DescriptorHeap> colorDescriptorsHeap;
		ComPtr<ID3D12DescriptorHeap> depthStencilDescriptorsHeap;
		ComPtr<ID3D12DescriptorHeap> depthDescriptorsHeap;
		ComPtr<ID3D12DescriptorHeap> stencilDescriptorsHeap;

	};

}
