#pragma once

#include "IGpuMemoryAllocator.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL GpuMemoryAllocatorOgl : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorOgl(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) override;
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& vertices) override;
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) override;
		OwnedPtr<GpuImage> CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;
		OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) override;
		OwnedPtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) override;

	};

}
