#pragma once

#include "IGpuMemoryAllocator.h"
#include "DescriptorDx12.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemoryAllocatorDx12 final : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorDx12(IGpu* device);

		OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;

		DescriptorDx12 GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	protected:

		OwnedPtr<IGpuVertexBuffer> _CreateVertexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment,
			TSize numVertices,
			const VertexInfo& vertexInfo) override;

		OwnedPtr<IGpuIndexBuffer> _CreateIndexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment,
			TSize numIndices) override;

		OwnedPtr<IGpuUniformBuffer> _CreateUniformBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) override;

		OwnedPtr<IGpuStorageBuffer> _CreateStorageBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) override;


		OwnedPtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() override;
		OwnedPtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() override;

		OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;

		OwnedPtr<IGpuMemoryBlock> CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);
		//OwnedPtr<IGpuMemoryBlock> CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) override;

		HashMap<D3D12_DESCRIPTOR_HEAP_TYPE, DynamicArray<DescriptorBlockDx12>> descriptorBlocks;

	};

}
