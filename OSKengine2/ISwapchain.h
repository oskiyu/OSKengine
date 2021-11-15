#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Un swapchain es una estructura encargada de manejar el cambio de imagenes que
	/// son representadas en el monitor.
	/// 
	/// La GPU trabaja en una sola imagen a la vez. El swapchain se encarga entonces
	/// de transmitir la imagen al monitor.
	/// </summary>
	class OSKAPI_CALL ISwapchain {

	public:

		~ISwapchain() = default;

		/// <summary>
		/// Devuelve el número de imágenes del swapchain.
		/// </summary>
		unsigned int GetImageCount() const;

		template <typename T> T* As() const {
			return (T*)this;
		}

	protected:

		unsigned int imageCount = 3;

	};

}
