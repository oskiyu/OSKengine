#pragma once

#include "OSKmacros.h"

enum VkImageLayout;

namespace OSK::GRAPHICS {

	enum class GpuImageLayout;

	VkImageLayout OSKAPI_CALL GetGpuImageLayoutVulkan(GpuImageLayout layout);

}
