#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "GpuImage.h"
#include "GpuInfo.h"
#include "DynamicArray.hpp"

namespace OSK {
	class RenderAPI;
}

namespace OSK::VULKAN {

	class OSK::RenderAPI;

	/// <summary>
	/// Representa un swapchain.
	/// Un swapchain es una estrucrura que se encarga de presentar las imágenes renderizadas al monitor.
	/// Hay un único swapchain.
	/// Para el render target final, las imágenes de la textura se reemplazan por las imágenes del swapchain.
	/// </summary>
	class OSKAPI_CALL Swapchain {

		friend class OSK::RenderAPI;

	public:

		/// <summary>
		/// Destruye el swapchain.
		/// </summary>
		~Swapchain();

		/// <summary>
		/// Devuelve el número de imágenes que componen el swapchain.
		/// </summary>
		uint32_t GetImageCount() const;

		/// <summary>
		/// Devuelve el formato del swapchain.
		/// </summary>
		VkFormat GetFormat() const;

	private:

		/// <summary>
		/// Establece el renderizador.
		/// </summary>
		Swapchain(RenderAPI* renderer);

		/// <summary>
		/// Crea el swapchain.
		/// Llama a: <para/>
		/// CreateSwapchain() <para/>
		/// AcquireImages() <para/>
		/// CreateImageViews()
		/// </summary>
		void Create();

		/// <summary>
		/// Crea el objeto del swapchain.
		/// </summary>
		void CreateSwapchain();

		/// <summary>
		/// Obtiene las imágenes para el swapchain.
		/// (Las imágenes son creadas por Vulkan).
		/// </summary>
		void AcquireImages();

		/// <summary>
		/// Crea los image view.
		/// </summary>
		void CreateImageViews();

		/// <summary>
		/// Recrea el swapchain, llamando a Close() y Create().
		/// </summary>
		void RecreateSwapchain();

		/// <summary>
		/// Elimina el swapchain.
		/// </summary>
		void Close();

		/// <summary>
		/// Obtiene el tamaño del swapchain.
		/// </summary>
		/// <param name="gpu">Info de la GPU usada.</param>
		VkExtent2D GetSupportedExtent(GpuInfo gpu) const;

		/// <summary>
		/// Devuelve el formato del swapchain.
		/// </summary>
		/// <param name="gpu">Info de la GPU usada.</param>
		VkSurfaceFormatKHR GetSupportedFormat(GpuInfo gpu) const;


		/// <summary>
		/// Swapchain nativo.
		/// </summary>
		VkSwapchainKHR swapchain;

		/// <summary>
		/// Views de las imágenes.
		/// </summary>
		std::vector<VkImageView> imageViews;

		/// <summary>
		/// Imágenes usadas.
		/// </summary>
		VkImage* images = nullptr;
		/// <summary>
		/// Número de imágenes.
		/// </summary>
		uint32_t imageCount;

		/// <summary>
		/// Formato del swapchain.
		/// </summary>
		VkFormat format;

		/// <summary>
		/// Tamaño del swapchain.
		/// </summary>
		VkExtent2D size;
		
		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

	};

}
