#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	enum class GpuSharedMemoryType;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;

	unsigned int OSKAPI_CALL GetGpuSharedMemoryTypeVk(GpuSharedMemoryType type);
	unsigned int OSKAPI_CALL GetGpuBufferUsageVk(GpuBufferUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageUsageVk(GpuImageUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageAspectVk(GpuImageUsage usage);

}
