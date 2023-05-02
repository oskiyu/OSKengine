#include "GpuMemoryAllocatorDx12.h"

#include "GpuMemoryBlockDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuImageDx12.h"
#include "GpuBuffer.h"
#include "GpuDx12.h"
#include "GpuMemoryTypesDx12.h"
#include "Vertex.h"
#include "GpuMemoryTypes.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "CommandListDx12.h"
#include "GpuImageUsage.h"
#include "FormatDx12.h"
#include "GpuImageDimensions.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryAllocatorDx12::GpuMemoryAllocatorDx12(IGpu* device)
	: IGpuMemoryAllocator(device) { 

	minVertexBufferAlignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	minIndexBufferAlignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	minUniformBufferAlignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	minStorageBufferAlignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockDx12::CreateNewBufferBlock(size, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockDx12::CreateNewImageBlock(image, device, sharedType, usage, image->GetNumLayers()).GetPointer();
}


OwnedPtr<GpuImage> GpuMemoryAllocatorDx12::CreateImage(const GpuImageCreateInfo& info) {
	auto output = new GpuImageDx12(info.resolution, info.dimension, info.usage, info.numLayers, info.format, info.msaaSamples, info.samplerDesc);
	output->_SetPhysicalSize({
		MATH::PrimerMultiploSuperior<uint32_t>(info.resolution.X, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT),
		info.resolution.Y,
		info.resolution.Z
		});

	auto block = GpuMemoryBlockDx12::CreateNewImageBlock(output, device, info.memoryType, info.usage, info.numLayers);

	output->SetBlock(block.GetPointer());

	return output;
}


OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	TSize bSize = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1000;

	if (size > bSize)
		bSize = size;

	return GpuMemoryBlockDx12::CreateNewBufferBlock(bSize, device, sharedType, usage).GetPointer();
}


OwnedPtr<IBottomLevelAccelerationStructure> GpuMemoryAllocatorDx12::_CreateBottomAccelerationStructure() {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

OwnedPtr<ITopLevelAccelerationStructure> GpuMemoryAllocatorDx12::_CreateTopAccelerationStructure() {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

DescriptorDx12 GpuMemoryAllocatorDx12::GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	if (!descriptorBlocks.HasValue(type))
		descriptorBlocks.Insert(type, {});

	auto& list = descriptorBlocks.Get(type);
	if (list.IsEmpty() || list.Peek().IsFull())
		list.Insert(DescriptorBlockDx12(type, 256, device->As<GpuDx12>()));
		
	return list.Peek().CreateDescriptor();
}
