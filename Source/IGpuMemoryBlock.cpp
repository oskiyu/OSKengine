#include "IGpuMemoryBlock.h"

#include "IGpuMemorySubblock.h"

#include "OSKengine.h"
#include "Logger.h"

#include "GpuMemoryExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuMemoryBlock::IGpuMemoryBlock(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage)
	: totalSize(reservedSize), availableSpace(reservedSize), type(type), usage(usage), device(device) { }

void IGpuMemoryBlock::RemoveSubblock(IGpuMemorySubblock* subblock) {
	std::lock_guard lock(m_subblockSearchMutex.mutex);
	subblocks.Remove(subblock);
	// Engine::GetLogger()->InfoLog("Subbloque quitado.");
}

USize64 IGpuMemoryBlock::GetAllocatedSize() const {
	return totalSize;
}

USize64 IGpuMemoryBlock::GetAvailableSpace() const {
	// std::lock_guard lock(m_subblockSearchMutex.mutex);
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

IGpuMemorySubblock* IGpuMemoryBlock::GetNextMemorySubblock(USize64 size, USize64 alignment) {
	std::lock_guard lock(m_subblockSearchMutex.mutex);

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

	OSK_ASSERT(size <= GetAvailableSpace(), GpuMemoryBlockNotEnoughSpaceException());
	
	USize64 finalOffset = currentOffset;
	USize64 extraOffset = 0;
	if (alignment != 0 && currentOffset != 0) {
		const USize64 base = currentOffset / alignment;
		finalOffset = (base + 1) * alignment;
		extraOffset = finalOffset - currentOffset;
	}

	output = CreateNewMemorySubblock(size, finalOffset).GetPointer();
	subblocks.Insert(output);
	availableSpace -= size + extraOffset;
	currentOffset += size + extraOffset;

	return output;
}
