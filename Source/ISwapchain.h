#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

#include "PresentMode.h"
#include "Format.h"

#include <array>

namespace OSK::GRAPHICS {


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

		OSK_DEFINE_AS(ISwapchain);

		virtual ~ISwapchain() = default;


		void TakeScreenshot();

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

		/// @brief Cambia el modo de presentaci�n del renderizador.
		/// @param mode Modo de sincronizaci�n vertical.
		virtual void SetPresentMode(PresentMode mode) = 0;

		/// <summary>
		/// Devuelve la imagen con el �ndice dado.
		/// </summary>
		/// 
		/// @pre El �ndice est� dentro de los l�mites.
		GpuImage* GetImage(unsigned int index) {
			return m_images[index].GetPointer();
		}
		const GpuImage* GetImage(unsigned int index) const {
			return m_images[index].GetPointer();
		}

		PresentMode GetCurrentPresentMode() const;

		/// <summary>
		/// Devuelve el formato de las im�genes del swapchain.
		/// </summary>
		Format GetColorFormat() const;

	protected:

		PresentMode m_presentMode = PresentMode::VSYNC_OFF;
		Format m_colorFormat = Format::UNKNOWN;

		IGpu* m_device = nullptr;
		unsigned int m_imageCount = NUM_RESOURCES_IN_FLIGHT;

		std::array<UniquePtr<GpuImage>, NUM_RESOURCES_IN_FLIGHT> m_images{};
		unsigned int m_currentFrameIndex = 0;

	};

}
