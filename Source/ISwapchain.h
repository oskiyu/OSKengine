#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "ResourcesInFlight.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"

#include "PresentMode.h"
#include "Format.h"

#include <array>

namespace OSK::GRAPHICS {

	class IGpu;

	/// @brief Un swapchain es una estructura encargada de manejar el cambio de imagenes que
	/// son representadas en el monitor.
	/// 
	/// La GPU trabaja en una sola imagen a la vez. El swapchain se encarga entonces
	/// de transmitir la imagen al monitor.
	class OSKAPI_CALL ISwapchain {

	public:

		OSK_DEFINE_AS(ISwapchain);

		virtual ~ISwapchain() = default;


		/// @brief Obtiene una captura de pantalla del juego
		/// y la guarda en disco.
		/// @param path Ruta + nombre de la imagen.
		/// 
		/// @post La imagen se guardara con el nombre "path_<año>_<mes>_<dia>_<hora><minuto><segundo>.png"
		/// 
		/// @todo Soporte para swapchain con formatos sin swizzle
		void TakeScreenshot(std::string_view path);


		/// @return Devuelve el número de imágenes del swapchain.
		/// 
		/// @details Será mayor que 0.
		/// @details Será menor que `NUM_RESOURCES_IN_FLIGHT`.
		USize32 GetImageCount() const;

		/// @return Índice de la imagen renderizada en un momento dado.
		unsigned int GetCurrentFrameIndex() const;


		/// @brief Envía la imagen renderizada al monitor.
		virtual void Present() = 0;

		/// @brief Cambia el modo de presentación del renderizador.
		/// @param mode Modo de sincronización vertical.
		void SetPresentMode(
			const IGpu& gpu,
			PresentMode mode);

		

		/// @param index Índice de la imagen dentro del swapchain.
		/// @return Imagen con el índice dado.
		/// 
		/// @pre @p index < `GetImageCount()`.
		GpuImage* GetImage(unsigned int index) {
			return m_images[index].GetPointer();

			/// @param index Índice de la imagen dentro del swapchain.
			/// @return Imagen con el índice dado.
			/// 
			/// @pre @p index < `GetImageCount()`.
		}
		const GpuImage* GetImage(unsigned int index) const {
			return m_images[index].GetPointer();
		}


		/// @brief Actualiza el swapchain para reflejar un
		/// cambio de resolución de pantalla.
		/// @param gpu GPU dueña del swapchain.
		/// @param newResolution Nueva resolución.
		/// 
		/// @pre @p newResolution != `Vector2ui::Zero`.
		virtual void Resize(
			const IGpu& gpu,
			Vector2ui newResolution) = 0;

		/// @return Modo de presentación actual.
		PresentMode GetCurrentPresentMode() const;

		/// @return Formato de las imágenes del swapchain.
		Format GetColorFormat() const;

	protected:

		/// @brief Inicializa las variables de la clase.
		/// @param mode Modo de presentación.
		/// @param format Formato de las imágenes del swapchain.
		/// @param queueIndices Índices de las colas de comandos que
		/// accederán a las imágenes del swapchain.
		/// 
		/// @pre @p queueIndices debe tener al menos una entrada.
		/// @pre @p queueIndices debe referenciar, al menos,
		/// la cola que se vaya a usar para operaciones
		/// de presentación.
		/// @pre @p queueIndices debe referenciar, al menos,
		/// la cola que se vaya a usar para operaciones
		/// renderizar sobre el swapchain.
		ISwapchain(
			PresentMode mode,
			Format format,
			std::span<const UIndex32> queueIndices);

		/// @brief Actualiza la variable de la clase.
		/// @param imageCount Número de imágenes del swapchain.
		/// 
		/// @note No ejecuta ninguna lógica de adquisición de imágenes.
		/// 
		/// @pre @p imageCount > 0.
		/// @pre @p imageCount < `NUM_RESOURCES_IN_FLIGHT`.
		void SetNumImagesInFlight(USize32 imageCount);

		/// @brief Establece la imagen del swapchain en el índice dado.
		/// @param image Imagen a establecer.
		/// @param index Índice de la imagen dentro del swapchain.
		/// 
		/// @pre @p index < `GetImageCount()`.
		void SetImage(
			OwnedPtr<GpuImage> image,
			UIndex32 index);

		/// @return Índices de las colas que usarán las imágenes
		/// del swapchain.
		std::span<const UIndex32> GetQueueIndices() const;

	private:

		USize32 m_numImagesInFlight = MAX_RESOURCES_IN_FLIGHT;

		PresentMode m_presentMode = PresentMode::VSYNC_OFF;
		Format m_colorFormat = Format::UNKNOWN;

		std::array<UniquePtr<GpuImage>, MAX_RESOURCES_IN_FLIGHT> m_images{};
		UIndex32 m_currentFrameIndex = 0;

		DynamicArray<UIndex32> m_queueIndices;

	};

}
