#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

enum VkFormat;

namespace OSK::GRAPHICS {
	
	enum class Format;

	/// <summary>
	/// Obtiene la representación nativa del formato para Vulkan.
	/// </summary>
	VkFormat GetFormatVk(Format format);

}

#endif
