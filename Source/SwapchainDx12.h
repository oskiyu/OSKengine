#pragma once

#include "ISwapchain.h"
#include "UniquePtr.hpp"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK::IO {
	class IDisplay;
}

namespace OSK::GRAPHICS {
	
	enum class Format;
	class CommandQueueDx12;
	class GpuImage;
	class GpuDx12;


	class OSKAPI_CALL SwapchainDx12 : public ISwapchain {

	public:

		SwapchainDx12(
			PresentMode mode,
			Format format,
			const GpuDx12& device,
			std::span<const UIndex32> queueIndices,
			const IO::IDisplay& display,
			IDXGIFactory4* factory);
		~SwapchainDx12() override;

		void Create(PresentMode mode, IGpu* device, Format format, const CommandQueueDx12& commandQueue, IDXGIFactory4* factory, const IO::IDisplay& display);
		
		void DeleteImages();
		void CreateImages(const IO::IDisplay& display);

		void Present() override;

		void UpdateFrameIndex();

		IDXGISwapChain3* GetSwapchain() const;
		ID3D12DescriptorHeap* GetRenderTargetMemory() const;
		ID3D12DescriptorHeap* GetDepthStencilMemory() const;

		void Resize(const IGpu& gpu, Vector2ui newResolution) override;

	private:

		ComPtr<IDXGISwapChain3> swapchain;

		ComPtr<ID3D12DescriptorHeap> renderTargetsDesc;
		ComPtr<ID3D12DescriptorHeap> depthTargetsDescHeap;

		ComPtr<ID3D12Heap> depthHeaps[3];
		UniquePtr<GpuImage> depthImages[3];

	};

}
