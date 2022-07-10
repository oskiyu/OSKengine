#pragma once

#include "ISwapchain.h"

enum VkColorSpaceKHR;

struct VkSwapchainKHR_T;
typedef VkSwapchainKHR_T* VkSwapchainKHR;
	
namespace OSK::IO {
	class Window;
}

namespace OSK::GRAPHICS {

	enum class Format;
	class GpuVulkan;
	class GpuImageVulkan;
	class RenderpassVulkan;
	class OSKAPI_CALL SwapchainVulkan : public ISwapchain {

	public:

		~SwapchainVulkan();

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene automáticamente el tamaño de las imágenes a partir del
		/// tamaño de la ventana.
		/// </summary>
		void Create(PresentMode mode, Format format, const GpuVulkan& device, const IO::Window& window);

		/// <summary>
		/// Recrea el swapchain con el tamaño de la ventana.
		/// Para cuando se ha cambiado de tamaño la ventana.
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
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVulkan& device);

		VkSwapchainKHR swapchain;
		const IO::Window* window = nullptr;
		const GpuVulkan* device = nullptr;

		Format format;

	};

}
