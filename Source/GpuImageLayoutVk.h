#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"
#include "VulkanTypedefs.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class GpuImageLayout;

	VkImageLayout OSKAPI_CALL GetGpuImageLayoutVk(GpuImageLayout layout);

}

#endif
