#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "DynamicArray.hpp"

#include "RenderpassAttachmentVk.h"
#include "RenderSubpassVk.h"

#include "VulkanTarget.h"
#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	template <VulkanTarget Target>
	class RenderpassVk {

	public:

		explicit RenderpassVk(
			const DynamicArray<RenderpassAttachmentVk>& attachments,
			const DynamicArray<RenderSubpassVk>& subpasses);
		~RenderpassVk();

		OSK_DISABLE_COPY(RenderpassVk);

		RenderpassVk(RenderpassVk&& other) noexcept;
		const RenderpassVk& operator=(RenderpassVk&& other) noexcept;

		VkRenderPass GetRenderpass() const;

	private:

		VkRenderPass m_renderpass = VK_NULL_HANDLE;

	};

}

#endif // OSK_USE_VULKAN_BACKEND
