#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "DynamicArray.hpp"

#include "RenderpassAttachmentVk.h"
#include "RenderSubpassVk.h"
#include "FramebufferVkMap.h"

#include "VulkanTarget.h"
#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuVk;

	/// @brief Encapsula un renderpass nativo de
	/// para Vulkan. Se usa si no se soporta el 
	/// renderizado dinámico.
	class RenderpassVk {

	public:

		explicit RenderpassVk(
			const GpuVk* gpu,
			const DynamicArray<RenderpassAttachmentVk>& attachments,
			const DynamicArray<RenderSubpassVk>& subpasses);
		~RenderpassVk();

		OSK_DISABLE_COPY(RenderpassVk);

		RenderpassVk(RenderpassVk&& other) noexcept;
		RenderpassVk& operator=(RenderpassVk&& other) noexcept;

		VkRenderPass GetRenderpass() const;

		FramebufferVk* GetFramebuffer(const FramebufferVkMapEntry& entry);

	private:

		VkRenderPass m_renderpass = VK_NULL_HANDLE;
		FramebufferVkMap m_framebuffers;

	};

}

#endif // OSK_USE_VULKAN_BACKEND
