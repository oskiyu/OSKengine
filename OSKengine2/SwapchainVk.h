#pragma once

#include "ISwapchain.h"

enum VkColorSpaceKHR;

struct VkSwapchainKHR_T;
typedef VkSwapchainKHR_T* VkSwapchainKHR;
	
namespace OSK::IO {
	class IDisplay;
}

namespace OSK::GRAPHICS {

	enum class Format;
	class GpuVk;
	class GpuImageVk;
	class RenderpassVulkan;
	class OSKAPI_CALL SwapchainVk final : public ISwapchain {

	public:

		~SwapchainVk();

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene autom�ticamente el tama�o de las im�genes a partir del
		/// tama�o de la ventana.
		/// </summary>
		void Create(
			PresentMode mode, 
			Format format, 
			const GpuVk& device, 
			const IO::IDisplay& display);

		/// <summary>
		/// Recrea el swapchain con el tama�o de la ventana.
		/// Para cuando se ha cambiado de tama�o la ventana.
		/// </summary>
		void Resize();

		void Present() override;

		VkSwapchainKHR GetSwapchain() const;

	private:

		void AcquireImages(unsigned int sizeX, unsigned int sizeY);
		void AcquireViews();

		/// <summary>
		/// Devuelve el mejor formato de colores soportado por el monitor.
		/// </summary>
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVk& device);

		VkSwapchainKHR swapchain = 0;
		const IO::IDisplay* display = nullptr;
		const GpuVk* device = nullptr;

	};

}