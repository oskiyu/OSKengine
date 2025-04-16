#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IGpuImage.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuImageDx12 : public GpuImage {

	public:

		GpuImageDx12(
			const GpuImageCreateInfo& info,
			const ICommandQueue* queue);

		void FillResourceDesc();
		const D3D12_RESOURCE_DESC& GetResourceDesc();
		void CreateResource(ID3D12Heap* memory, USize64 memoryOffset);

		// Swapchain
		void _SetResource(ComPtr<ID3D12Resource> resource);

		ID3D12Resource* GetResource() const;

		void SetDebugName(const std::string& name) override;

	protected:

		UniquePtr<IGpuImageView> CreateView(const GpuImageViewConfig& config) const override;

	private:

		D3D12_RESOURCE_DESC resourceDesc{};

		ComPtr<ID3D12Resource> resource;

	};

}

#endif
