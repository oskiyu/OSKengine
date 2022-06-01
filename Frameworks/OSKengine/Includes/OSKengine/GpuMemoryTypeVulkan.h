#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	enum class GpuSharedMemoryType;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;

	unsigned int OSKAPI_CALL GetGpuSharedMemoryTypeVulkan(GpuSharedMemoryType type);
	unsigned int OSKAPI_CALL GetGpuBufferUsageVulkan(GpuBufferUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageUsageVulkan(GpuImageUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageAspectVulkan(GpuImageUsage usage);

}
