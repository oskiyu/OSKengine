#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK {

	class OSKAPI_CALL GpuMemoryAllocatorDx12 : public IGpuMemoryAllocator {

	public:

		//OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<GpuImage> CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;
		GpuMemoryAllocatorDx12(IGpu* device);

	protected:


		//OwnedPtr<IGpuMemoryBlock> CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		//OwnedPtr<IGpuMemoryBlock> CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) override;

	};

}
