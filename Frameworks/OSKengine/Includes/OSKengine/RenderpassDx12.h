#pragma once

#include "IRenderpass.h"

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class SwapchainDx12;

	class OSKAPI_CALL RenderpassDx12 : public IRenderpass {

	public:

		RenderpassDx12(RenderpassType type);
		~RenderpassDx12();

		void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) override;

		void SetSwapchain(SwapchainDx12* swapchain);
		SwapchainDx12* GetSwapchain() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetDescpriptor(TSize index) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilDescpriptor(TSize index) const;

	private:

		SwapchainDx12* swapchain = nullptr;

		D3D12_CPU_DESCRIPTOR_HANDLE colorDescs[3];
		D3D12_CPU_DESCRIPTOR_HANDLE depthDescs[3];
		D3D12_CPU_DESCRIPTOR_HANDLE finalDescs[3];

	};

}
