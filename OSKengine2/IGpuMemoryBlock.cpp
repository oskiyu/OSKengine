#include "IGpuMemoryBlock.h"

#include "IGpuMemorySubblock.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuMemoryBlock::IGpuMemoryBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage)
	: totalSize(reservedSize), availableSpace(reservedSize), device(device), type(type), usage(usage) { }

void IGpuMemoryBlock::RemoveSubblock(IGpuMemorySubblock* subblock) {
	subblocks.Remove(subblock);
}

IGpuMemoryBlock::TSize IGpuMemoryBlock::GetAllocatedSize() const {
	return totalSize;
}

IGpuMemoryBlock::TSize IGpuMemoryBlock::GetAvailableSpace() const {
	return availableSpace;
}

bool IGpuMemoryBlock::ReusableMemorySubblock::operator==(const ReusableMemorySubblock& other) const {
	return offset == other.offset;
}

GpuSharedMemoryType IGpuMemoryBlock::GetShareType() const {
	return type;
}

GpuMemoryUsage IGpuMemoryBlock::GetUsageType() const {
	return usage;
}

IGpuMemorySubblock* IGpuMemoryBlock::GetNextMemorySubblock(TSize size) {
	IGpuMemorySubblock* output = nullptr;

	bool isReused = false;

	for (auto& reuse : reusableSubblocks) {
		if (size < reuse.size) {
			output = CreateNewMemorySubblock(size, reuse.offset).GetPointer();

			reusableSubblocks.Remove(reuse);

			isReused = true;

			break;
		}
	}

	OSK_ASSERT(size <= GetAvailableSpace(), "No se ha escogido un bloque de memoria con suficiente espacio libre.");
	
	output = CreateNewMemorySubblock(size, currentOffset).GetPointer();
	subblocks.Insert(output);
	availableSpace -= size;
	currentOffset += size;

	return output;
}

IGpu* IGpuMemoryBlock::GetDevice() const {
	return device;
}
