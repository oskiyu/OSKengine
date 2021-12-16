#pragma once

#include "OSKmacros.h"

enum VkImageLayout;

namespace OSK {

	enum class GpuImageLayout;

	VkImageLayout OSKAPI_CALL GetGpuImageLayoutVulkan(GpuImageLayout layout);

}
