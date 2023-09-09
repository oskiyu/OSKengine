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

		~SwapchainVk() override;

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene automáticamente el tamaño de las imágenes a partir del
		/// tamaño de la ventana.
		/// </summary>
		/// 
		/// @throws SwapchainCreationException Si ocurre algún error.
		void Create(
			PresentMode mode, 
			Format format, 
			const GpuVk& device, 
			const IO::IDisplay& display);

		/// <summary>
		/// Recrea el swapchain con el tamaño de la ventana.
		/// Para cuando se ha cambiado de tamaño la ventana.
		/// </summary>
		void Resize();

		void Present() override;

		VkSwapchainKHR GetSwapchain() const;

	private:

		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireImages(unsigned int sizeX, unsigned int sizeY);
		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireViews();

		/// <summary>
		/// Devuelve el mejor formato de colores soportado por el monitor.
		/// </summary>
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVk& device);

		VkSwapchainKHR m_swapchain = 0;
		const IO::IDisplay* m_display = nullptr;
		const GpuVk* m_device = nullptr;

	};

}
