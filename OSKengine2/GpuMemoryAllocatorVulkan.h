#pragma once

#include "IGpuMemoryAllocator.h"

struct VkDeviceMemory_T;
typedef VkDeviceMemory_T* VkDeviceMemory;

namespace OSK {

	class OSKAPI_CALL GpuMemoryAllocatorVulkan : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorVulkan(IGpu* device);

		//OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<GpuImage> CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
