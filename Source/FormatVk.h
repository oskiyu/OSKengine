#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {
	
	enum class Format;

	VkFormat GetFormatVk(Format format);
	Format FormatVkToEngine(VkFormat format);
}

#endif
