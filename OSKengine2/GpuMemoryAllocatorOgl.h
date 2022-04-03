#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemoryAllocatorOgl : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorOgl(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) override;
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& vertices) override;
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) override;
		OwnedPtr<GpuImage> CreateImage(const Vector2ui& size, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, bool singleSample, GpuImageSamplerDesc samplerDesc) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;

	protected:

		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
