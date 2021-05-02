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
		/// <param name="loadOp">Qu� se har� al cargar la imagen.</param>
		/// <param name="stroeOp">Qu� se har� al guardar la imagen.</param>
		/// <param name="layout">Layout de la imagen.</param>
		void AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp stroeOp, VkImageLayout layout);

		/// <summary>
		/// Crea la referencia del attachment.
		/// </summary>
		/// <param name="attachment">N�mero del attachment.</param>
		/// <param name="use">Layout que tendr�.</param>
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