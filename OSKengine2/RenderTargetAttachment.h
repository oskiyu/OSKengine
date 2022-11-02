#pragma once

#include "IMaterialSlot.h"

#include "RenderTargetAttachmentInfo.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa una imagen individual que se escribir� en un proceso
	/// de renderizado.
	/// </summary>
	class RenderTargetAttachment {

	public:

		/// <summary>
		/// Attachment vac�o, no usable.
		/// Crear con RenderTargetAttachment::Create o RenderTargetAttachment::Initialize.
		/// </summary>
		RenderTargetAttachment();

		/// <summary> Crea un attachment con la informaci�n dada. </summary>
		/// <param name="info">Informaci�n de la imagen.</param>
		/// <param name="resolution">Resoluci�n, en p�xeles.</param>
		static RenderTargetAttachment Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);


		/// <summary> Inicializa el attachment para su uso. </summary>
		/// <param name="info">Informaci�n de la imagen.</param>
		/// <param name="resolution">Resoluci�n, en p�xeles.</param>
		void Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);
		
		/// <summary> Cambia de tama�o la imagen. </summary>
		/// <param name="resolution">Nueva resoluci�n, en p�xeles.</param>
		/// 
		/// @pre Debe haberse inicializado con RenderTargetAttachment::Create / RenderTargetAttachment::Initialize.
		/// @note Si las im�genes del attachment se usaban en un material slot,
		/// este deber� actualizar sus recursos de esta imagen.
		void Resize(const Vector2ui& resolution);


		/// <summary> Devuelve la informaci�n con la que se cre� la imagen. </summary>
		const RenderTargetAttachmentInfo& GetInfo() const;

		/// <summary> Devuelve la imagen en el �ndice dado. </summary>
		/// <param name="resourceIndex">�ndice del recurso / frame.</param>
		/// <returns>Null si no se inicializ� previamente.</returns>
		/// 
		/// @pre 0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT.
		GpuImage* GetImage(TIndex resourceIndex) const;

	private:

		RenderTargetAttachmentInfo info;
		UniquePtr<GpuImage> images[NUM_RESOURCES_IN_FLIGHT]{};

	};

}
