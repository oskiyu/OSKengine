#pragma once

#include "ISwapchain.h"
#include "UniquePtr.hpp"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace OSK::IO {
	class Window;
}

namespace OSK::GRAPHICS {
	
	enum class Format;
	class CommandQueueDx12;
	class GpuImage;

	/// <summary>
	/// Un swapchain es una estructura encargada de manejar el cambio de imagenes que
	/// son representadas en el monitor.
	/// 
	/// La GPU trabaja en una sola imagen a la vez. El swapchain se encarga entonces
	/// de transmitir la imagen al monitor.
	/// </summary>
	class OSKAPI_CALL SwapchainDx12 : public ISwapchain {

	public:

		~SwapchainDx12();

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene automáticamente el tamaño de las imágenes a partir del
		/// tamaño de la ventana.
		/// </summary>
		/// <param name="commandQueue">Al cambiar de imagen, el swapchain
		/// de DirectX 12 fuerza un flush de la cola de comandos.</param>
		void Create(PresentMode mode, IGpu* device, Format format, const CommandQueueDx12& commandQueue, IDXGIFactory4* factory, const IO::Window& window);
		
		void DeleteImages();
		void CreateImages(const IO::Window& window);

		void Present() override;

		void UpdateFrameIndex();

		IDXGISwapChain3* GetSwapchain() const;
		ID3D12DescriptorHeap* GetRenderTargetMemory() const;
		ID3D12DescriptorHeap* GetDepthStencilMemory() const;

	private:

		ComPtr<IDXGISwapChain3> swapchain;

		ComPtr<ID3D12DescriptorHeap> renderTargetsDesc;
		ComPtr<ID3D12DescriptorHeap> depthTargetsDescHeap;

		ComPtr<ID3D12Heap> depthHeaps[3];
		UniquePtr<GpuImage> depthImages[3];

	};

}
