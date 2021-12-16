#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"

namespace OSK {

	class IGpu;
	class GpuImage;

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
		unsigned int GetCurrentFrameIndex() const;

		virtual void Present() = 0;

		template <typename T> T* As() const requires std::is_base_of_v<ISwapchain, T> {
			return (T*)this;
		}

		GpuImage* GetImage(unsigned int index) const;

	protected:

		IGpu* device = nullptr;
		unsigned int imageCount = 3;

		UniquePtr<GpuImage> images[3];
		unsigned int currentFrameIndex;

	};

}
