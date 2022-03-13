#include "IGpuMemoryAllocator.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include "IGpuDataBuffer.h"
#include "IGpuImage.h"
#include "Format.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuMemoryAllocator::TSize IGpuMemoryAllocator::SizeOfMemoryBlockInMb = 128;

bool GpuBufferMemoryBlockInfo::operator==(const GpuBufferMemoryBlockInfo& other) const {
	return size == other.size && usage == other.usage && sharedType == other.sharedType;
}

IGpuMemoryAllocator::IGpuMemoryAllocator(IGpu* device) : device(device) {
	bufferMemoryBlocks = {};
}

IGpuMemoryAllocator::~IGpuMemoryAllocator() {
	for (auto& list : bufferMemoryBlocks)
		for (auto& block : list.second)
			delete block.GetPointer();

	for (auto& i : imageMemoryBlocks)
		delete i.GetPointer();
}

void IGpuMemoryAllocator::RemoveImageBlock(IGpuMemoryBlock* iblock) {
	OwnedPtr block = iblock;

	TSize i = 0;
	for (auto it = imageMemoryBlocks.begin(); it != imageMemoryBlocks.end(); ++it) {
		if (it.GetNode()->GetValue().GetPointer() == iblock)
			break;

		i++;
	}

	if (i != imageMemoryBlocks.GetSize())
		imageMemoryBlocks.RemoveAt(i);
}

void IGpuMemoryAllocator::Free() {
	for (auto& list : bufferMemoryBlocks) {
		for (auto& i : list.second)
			i->Free();

		list.second.Free();
	}

	for (auto i : imageMemoryBlocks)
		i->Free();

	imageMemoryBlocks.Free();
}

/*IGpuMemoryBlock* IGpuMemoryAllocator::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	auto it = bufferMemoryBlocks.Find({ size, usage, sharedType });

	OSK_ASSERT(false, "Deprecated: use virtual function.");

	if (it.IsEmpty()) {
		auto i = CreateNewBufferMemoryBlock(size, usage, sharedType);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i);

		return i.GetPointer();
	}
	else {
		auto& list = it.GetValue().second;

		for (auto i : list)
			if (i->GetAvailableSpace() >= size)
				return i.GetPointer();

		auto i = CreateNewBufferMemoryBlock(size, usage, sharedType);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i);

		return i.GetPointer();
	}

	return nullptr;
}*/

IGpuMemorySubblock* IGpuMemoryAllocator::GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size);
}

/*OwnedPtr<GpuDataBuffer> IGpuMemoryAllocator::CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return new GpuDataBuffer(GetNextBufferMemorySubblock(size, usage, sharedType), size, 0);
}*/
