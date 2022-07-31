#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::GRAPHICS {

	enum class PresentMode;

	class IGpu;

	/// <summary>
	/// Un swapchain es una estructura encargada de manejar el cambio de imagenes que
	/// son representadas en el monitor.
	/// 
	/// La GPU trabaja en una sola imagen a la vez. El swapchain se encarga entonces
	/// de transmitir la imagen al monitor.
	/// </summary>
	class OSKAPI_CALL ISwapchain {

	public:

		virtual ~ISwapchain() = default;

		/// <summary>
		/// Devuelve el número de imágenes del swapchain.
		/// </summary>
		unsigned int GetImageCount() const;

		/// <summary>
		/// Devuelve el índice de la imagen renderizada en un momento dado.
		/// </summary>
		unsigned int GetCurrentFrameIndex() const;

		/// <summary>
		/// Envía la imagen renderizada al monitor.
		/// </summary>
		virtual void Present() = 0;

		template <typename T> T* As() const requires std::is_base_of_v<ISwapchain, T> {
			return (T*)this;
		}

		/// <summary>
		/// Devuelve la imagen con el índice dado.
		/// </summary>
		/// 
		/// @pre El índice está dentro de los límites.
		GpuImage* GetImage(unsigned int index) const;

		PresentMode GetCurrentPresentMode() const;

		/// <summary>
		/// Devuelve el formato de las imágenes del swapchain.
		/// </summary>
		Format GetColorFormat() const;

	protected:

		PresentMode mode{};
		Format colorFormat;

		IGpu* device = nullptr;
		unsigned int imageCount = 3;

		UniquePtr<GpuImage> images[3];
		unsigned int currentFrameIndex = 0;

	};

}
