#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK {

	class OSKAPI_CALL GpuMemoryAllocatorOgl : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorOgl(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) override;
		OwnedPtr<GpuImage> CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;

	protected:

		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
