#pragma once

#include "IGpuImage.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuImageDx12 : public GpuImage {

	public:

		GpuImageDx12(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format);

		void SetResource(const ComPtr<ID3D12Resource>& resource);

		void _SetSampledDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap);
		void _SetRenderTargetDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap);
		void _SetDepthDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap);

		ID3D12Resource* GetResource() const;

		/// <summary>
		/// Devuelve la memoria del descriptor para uso como textura.
		/// Null si no ser� usado de esa manera.
		/// </summary>
		ID3D12DescriptorHeap* GetSampledDescriptorHeap() const;

		/// <summary>
		/// Devuelve la memoria del descriptor para uso como render target.
		/// Null si no ser� usado de esa manera.
		/// </summary>
		ID3D12DescriptorHeap* GetRenderTargetDescriptorHeap() const;

		/// <summary>
		/// Devuelve la memoria del descriptor para uso como depth/stencil target.
		/// Null si no ser� usado de esa manera.
		/// </summary>
		ID3D12DescriptorHeap* GetDepthDescriptorHeap() const;

	private:

		ComPtr<ID3D12Resource> resource;

		ComPtr<ID3D12DescriptorHeap> sampledDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> renderTargetDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> depthDescriptorHeap;

	};

}
