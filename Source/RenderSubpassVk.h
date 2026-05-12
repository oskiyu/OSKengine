#pragma once

#include "ApiCall.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>
#include "DynamicArray.hpp"
#include <optional>

#include "RenderpassAttachmentVk.h"

namespace OSK::GRAPHICS {

	/// @brief Encapsula un subpase de un renderpass.
	/// Se usa cuando no se soporta el renderizado 
	/// dinámico.
	class OSKAPI_CALL RenderSubpassVk {

	public:

		explicit RenderSubpassVk(
			VkPipelineBindPoint bindPoint,
			const DynamicArray<RenderpassAttachmentVk>& colorAttachments,
			std::optional<RenderpassAttachmentVk> depthAttachment);

		VkSubpassDescription GetDescription() const;
		const DynamicArray<VkSubpassDependency>& GetDependencies() const;

	private:

		void SetColorAttachments(const DynamicArray<RenderpassAttachmentVk>& attachments);

		VkSubpassDescription m_description{};
		DynamicArray<RenderpassAttachmentVk> m_attachments{};
		std::optional<RenderpassAttachmentVk> m_depthAttachment;
		DynamicArray<VkSubpassDependency> m_dependencies{};
		DynamicArray<VkAttachmentReference> m_references{};
		VkAttachmentReference m_depthReference{};

	};

}

#endif // OSK_USE_VULKAN_BACKEND
