#pragma once

#include "ApiCall.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>
#include "DynamicArray.hpp"

#include "RenderpassAttachmentVk.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL RenderSubpassVk {

	public:

		explicit RenderSubpassVk(
			VkPipelineBindPoint bindPoint,
			const DynamicArray<RenderpassAttachmentVk>& colorAttachments,
			const RenderpassAttachmentVk& depthAttachment);

		VkSubpassDescription GetDescription() const;
		const DynamicArray<VkSubpassDependency> GetDependencies() const;

	private:

		void SetColorAttachments(const DynamicArray<RenderpassAttachmentVk>& attachments);

		VkSubpassDescription m_description{};
		DynamicArray<RenderpassAttachmentVk> m_attachments{};
		RenderpassAttachmentVk m_depthAttachment;
		DynamicArray<VkSubpassDependency> m_dependencies{};
		DynamicArray<VkAttachmentReference> m_references{};

	};

}

#endif // OSK_USE_VULKAN_BACKEND
