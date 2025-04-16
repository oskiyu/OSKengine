#pragma once

#include "IMaterialSlot.h"

#include "RenderTargetAttachmentInfo.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include <array>

namespace OSK::GRAPHICS {

	/// @brief Representa una imagen individual que se escribirá en un proceso
	/// de renderizado.
	class RenderTargetAttachment {

	public:

		/// @brief Attachment vacío, no usable.
		/// Crear con RenderTargetAttachment::Create o RenderTargetAttachment::Initialize.
		RenderTargetAttachment() = default;

		/// @brief Crea un attachment con la información dada.
		/// @param info Información de la imagen.
		/// @param resolution Resolución, en píxeles.
		static RenderTargetAttachment Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);


		/// @brief Inicializa el attachment para su uso.
		/// @param info Información de la imagen.
		/// @param resolution Resolución, en píxeles.
		void Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);
		
		/// @brief Cambia de tamaño la imagen.
		/// @param resolution Nueva resolución, en píxeles.
		/// 
		/// @pre Debe haberse inicializado con RenderTargetAttachment::Create / RenderTargetAttachment::Initialize.
		/// @note Si las imágenes del attachment se usaban en un material slot,
		/// este deberá actualizar sus recursos de esta imagen.
		void Resize(const Vector2ui& resolution);


		/// @return Información con la que se creó la imagen.
		const RenderTargetAttachmentInfo& GetInfo() const;

		/// @return Imagen (null si no se inicializó previamente).
		GpuImage* GetImage();

		/// @return Imagen (null si no se inicializó previamente).
		const GpuImage* GetImage() const;

	private:

		RenderTargetAttachmentInfo m_info;
		UniquePtr<GpuImage> m_image;

	};

}
