#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::GRAPHICS {

	enum class PresentMode;

	class IGpu;
	class IRenderpass;

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
		/// Devuelve el n�mero de im�genes del swapchain.
		/// </summary>
		unsigned int GetImageCount() const;

		/// <summary>
		/// Devuelve el �ndice de la imagen renderizada en un momento dado.
		/// </summary>
		unsigned int GetCurrentFrameIndex() const;

		/// <summary>
		/// Env�a la imagen renderizada al monitor.
		/// </summary>
		virtual void Present() = 0;

		template <typename T> T* As() const requires std::is_base_of_v<ISwapchain, T> {
			return (T*)this;
		}

		/// <summary>
		/// Devuelve la imagen con el �ndice dado.
		/// </summary>
		/// 
		/// @pre El �ndice est� dentro de los l�mites.
		GpuImage* GetImage(unsigned int index) const;

		/// <summary>
		/// Establece el renderpass que representa el renderizado a la imagen final.
		/// </summary>
		void SetTargetRenderpass(IRenderpass* renderpass);

		/// <returns>Renderpass que representa el renderizado a la imagen final</returns>
		IRenderpass* GetTargetRenderpass() const;

		PresentMode GetCurrentPresentMode() const;

	protected:

		PresentMode mode{};

		IGpu* device = nullptr;
		unsigned int imageCount = 3;

		UniquePtr<GpuImage> images[3];
		unsigned int currentFrameIndex = 0;
		IRenderpass* targetRenderpass = nullptr;

	};

}