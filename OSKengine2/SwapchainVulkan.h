#pragma once

#include "ISwapchain.h"

enum VkColorSpaceKHR;

struct VkSwapchainKHR_T;
typedef VkSwapchainKHR_T* VkSwapchainKHR;

namespace OSK {

	class GpuVulkan;
	class Window;
	enum class Format;

	class OSKAPI_CALL SwapchainVulkan : public ISwapchain {

	public:

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene automáticamente el tamaño de las imágenes a partir del
		/// tamaño de la ventana.
		/// </summary>
		void Create(Format format, const GpuVulkan& device, const Window& window);
		void Resize();

		VkSwapchainKHR GetSwapchain() const;

	private:

		/// <summary>
		/// Devuelve el mejor formato de colores soportado por el monitor.
		/// </summary>
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVulkan& device);

		VkSwapchainKHR swapchain;
		const Window* window = nullptr;
		const GpuVulkan* device = nullptr;

		Format format;

	};

}
