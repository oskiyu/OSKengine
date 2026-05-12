#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class RenderpassAttachmentVk {

	public:

		RenderpassAttachmentVk(
			VkFormat targetFormat,
			VkAttachmentLoadOp loadOp,
			VkAttachmentStoreOp storeOp,
			VkImageLayout startLayout,
			VkImageLayout endLayout);

		VkAttachmentDescription GetDescription() const;

	private:

		VkAttachmentDescription m_description{};
	};

}

#endif // OSK_USE_VULKAN_BACKEND
