#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK {

	class OSKAPI_CALL GpuMemoryAllocatorOgl : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorOgl(IGpu* device);

		OwnedPtr<GpuImage> CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
