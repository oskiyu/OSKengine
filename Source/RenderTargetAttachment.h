#pragma once

#include "IMaterialSlot.h"

#include "RenderTargetAttachmentInfo.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include <array>

namespace OSK::GRAPHICS {

	/// @brief Representa una imagen individual que se escribir� en un proceso
	/// de renderizado.
	class RenderTargetAttachment {

	public:

		/// @brief Attachment vac�o, no usable.
		/// Crear con RenderTargetAttachment::Create o RenderTargetAttachment::Initialize.
		RenderTargetAttachment() = default;

		/// @brief Crea un attachment con la informaci�n dada.
		/// @param info Informaci�n de la imagen.
		/// @param resolution Resoluci�n, en p�xeles.
		static RenderTargetAttachment Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);


		/// @brief Inicializa el attachment para su uso.
		/// @param info Informaci�n de la imagen.
		/// @param resolution Resoluci�n, en p�xeles.
		void Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution);
		
		/// @brief Cambia de tama�o la imagen.
		/// @param resolution Nueva resoluci�n, en p�xeles.
		/// 
		/// @pre Debe haberse inicializado con RenderTargetAttachment::Create / RenderTargetAttachment::Initialize.
		/// @note Si las im�genes del attachment se usaban en un material slot,
		/// este deber� actualizar sus recursos de esta imagen.
		void Resize(const Vector2ui& resolution);


		/// @return Informaci�n con la que se cre� la imagen.
		const RenderTargetAttachmentInfo& GetInfo() const;

		/// @return Imagen (null si no se inicializ� previamente).
		GpuImage* GetImage();

		/// @return Imagen (null si no se inicializ� previamente).
		const GpuImage* GetImage() const;

	private:

		RenderTargetAttachmentInfo m_info;
		UniquePtr<GpuImage> m_image;

	};

}
