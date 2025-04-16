#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IGpuMemoryAllocator.h"
#include "DescriptorDx12.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemoryAllocatorDx12 final : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorDx12(IGpu* device);

		UniquePtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;

		DescriptorDx12 GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	protected:


		UniquePtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() override;
		UniquePtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() override;

		UniquePtr<IGpuMemoryBlock> CreateNewBufferBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		UniquePtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;

		UniquePtr<IGpuMemoryBlock> CreateNewBufferMemoryBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);
		//OwnedPtr<IGpuMemoryBlock> CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) override;

		std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, DynamicArray<DescriptorBlockDx12>> descriptorBlocks;

	};

}

#endif
