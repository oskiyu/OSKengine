#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemoryAllocatorDx12 : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorDx12(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices) override;
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& vertices) override;
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) override;
		OwnedPtr<GpuImage> CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;

	protected:

		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<IGpuMemoryBlock> CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);
		//OwnedPtr<IGpuMemoryBlock> CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) override;

	};

}
