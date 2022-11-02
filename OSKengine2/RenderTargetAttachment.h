#pragma once

#include "IMaterialSlot.h"

#include "RenderTargetAttachmentInfo.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa una imagen individual que se escribirá en un proceso
	/// de renderizado.
	/// </summary>
	class RenderTargetAttachment {

	public:

		/// <summary>
		/// Attachment vacío, no usable.
		/// Crear con RenderTargetAttachment::Create o RenderTargetAttachment::Initialize.
		/// </summary>
		RenderTargetAttachment();

		/// <summary> Crea un attachment con la información dada. </summary>
		/// <param name="info">Información de la imagen.</param>
		/// <param name="resolution">Resolución, en píxeles.</param>
		static RenderTargetAttachment Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);


		/// <summary> Inicializa el attachment para su uso. </summary>
		/// <param name="info">Información de la imagen.</param>
		/// <param name="resolution">Resolución, en píxeles.</param>
		void Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);
		
		/// <summary> Cambia de tamaño la imagen. </summary>
		/// <param name="resolution">Nueva resolución, en píxeles.</param>
		/// 
		/// @pre Debe haberse inicializado con RenderTargetAttachment::Create / RenderTargetAttachment::Initialize.
		/// @note Si las imágenes del attachment se usaban en un material slot,
		/// este deberá actualizar sus recursos de esta imagen.
		void Resize(const Vector2ui& resolution);


		/// <summary> Devuelve la información con la que se creó la imagen. </summary>
		const RenderTargetAttachmentInfo& GetInfo() const;

		/// <summary> Devuelve la imagen en el índice dado. </summary>
		/// <param name="resourceIndex">Índice del recurso / frame.</param>
		/// <returns>Null si no se inicializó previamente.</returns>
		/// 
		/// @pre 0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT.
		GpuImage* GetImage(TIndex resourceIndex) const;

	private:

		RenderTargetAttachmentInfo info;
		UniquePtr<GpuImage> images[NUM_RESOURCES_IN_FLIGHT]{};

	};

}
