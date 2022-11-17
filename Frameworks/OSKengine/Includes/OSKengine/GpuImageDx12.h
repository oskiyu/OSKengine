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

		void FillResourceDesc();
		const D3D12_RESOURCE_DESC& GetResourceDesc();
		void CreateResource(ID3D12Heap* memory, TSize memoryOffset);

		// Swapchain
		void _SetResource(ComPtr<ID3D12Resource> resource);

		ID3D12Resource* GetResource() const;

		void SetDebugName(const std::string& name) override;

	protected:

		OwnedPtr<IGpuImageView> CreateView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const override;

	private:

		D3D12_RESOURCE_DESC resourceDesc{};

		ComPtr<ID3D12Resource> resource;

	};

}
