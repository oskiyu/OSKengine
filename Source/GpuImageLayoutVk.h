#pragma once

#include "ApiCall.h"
#include "VulkanTypedefs.h"

enum VkImageLayout;

namespace OSK::GRAPHICS {

	enum class GpuImageLayout;

	VkImageLayout OSKAPI_CALL GetGpuImageLayoutVk(GpuImageLayout layout);

}
