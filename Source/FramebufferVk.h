#pragma once

#include "ApiCall.h"
#include "Platforms.h"

#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>
#include <span>

#include "GpuImageViewVk.h"
#include "GpuVk.h"

namespace OSK::GRAPHICS {

	class RenderpassVk;

	/// @brief Referencia a las imágenes sobre las
	/// que renderizará un renderpass nativo
	/// de Vulkan.
	class OSKAPI_CALL FramebufferVk {

	public:

		/// @brief Crea el framebuffer.
		/// @param device GPU sobre la que se creará.
		/// @param renderpass Pase de renderizado que usará este framebuffer.
		/// @param views Vistas de las imágenes sobre la que se renderizará.
		FramebufferVk(const GpuVk& device, const RenderpassVk& renderpass, std::span<const GpuImageViewVk* const> views);
		~FramebufferVk();

		OSK_DISABLE_COPY(FramebufferVk);

		FramebufferVk(FramebufferVk&&) noexcept;
		FramebufferVk& operator=(FramebufferVk&&) noexcept;

		VkFramebuffer GetFramebuffer() const;

	private:

		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;

	};

}

#endif // OSK_USE_VULKAN_BACKEND
