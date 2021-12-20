#include "GpuMemoryAllocatorDx12.h"

#include "GpuMemoryBlockDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuImageDx12.h"

using namespace OSK;

GpuMemoryAllocatorDx12::GpuMemoryAllocatorDx12(IGpu* device)
	: IGpuMemoryAllocator(device) { 
	
}

OwnedPtr<GpuImage> GpuMemoryAllocatorDx12::CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	auto output = new GpuImageDx12(sizeX, sizeY, format);

	auto block = GpuMemoryBlockDx12::CreateNewImageBlock(output, device, sharedType, usage);
	//auto subblock = block->GetNextMemorySubblock(block->GetAllocatedSize());

	output->SetBlock(block.GetPointer());	
	output->SetResource(output->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource());

	return output;
}

/*OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	TSize bSize = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1000;
	if (size > bSize)
		bSize = size;

	return GpuMemoryBlockDx12::CreateNewBufferBlock(bSize, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return GpuMemoryBlockDx12::CreateNewImageBlock(image, device, type, imageUSage).GetPointer();
}
*/
