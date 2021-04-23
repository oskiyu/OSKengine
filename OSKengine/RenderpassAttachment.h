#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	/// <summary>
	/// Representa los attachments del renderpass.
	/// Representa una imagen que se va a usar en el renderpass.
	/// </summary>
	class OSKAPI_CALL RenderpassAttachment {

	public:

		/// <summary>
		/// Establece el attachment.
		/// </summary>
		/// <param name="format">Formato.</param>
		/// <param name="loadOp">Qué se hará al cargar la imagen.</param>
		/// <param name="stroeOp">Qué se hará al guardar la imagen.</param>
		/// <param name="layout">Layout de la imagen.</param>
		void AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp stroeOp, VkImageLayout layout);

		/// <summary>
		/// Crea la referencia del attachment.
		/// </summary>
		/// <param name="attachment">Número del attachment.</param>
		/// <param name="use">Layout que tendrá.</param>
		void CreateReference(const uint32_t& attachment, VkImageLayout use);

		/// <summary>
		/// Attachment.
		/// </summary>
		VkAttachmentDescription attahcmentDesc;

		/// <summary>
		/// Referncia.
		/// </summary>
		VkAttachmentReference reference;

	};

}