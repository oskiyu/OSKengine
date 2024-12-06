#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"

namespace OSK::GRAPHICS {

	enum class GpuSharedMemoryType;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;

	unsigned int OSKAPI_CALL GetGpuSharedMemoryTypeVk(GpuSharedMemoryType type);
	unsigned int OSKAPI_CALL GetGpuBufferUsageVk(GpuBufferUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageUsageVk(GpuImageUsage usage);
	unsigned int OSKAPI_CALL GetGpuImageAspectVk(GpuImageUsage usage);

}

#endif
