#include "IGpuMemorySubblock.h"

#include "IGpuMemoryBlock.h"

using namespace OSK;

IGpuMemorySubblock::IGpuMemorySubblock(IGpuMemoryBlock* block, TSize size, TSize offset) 
	: ownerBlock(block), reservedSize(size), totalOffsetFromBlock(offset) {
	
}

IGpuMemorySubblock::~IGpuMemorySubblock() {
	ownerBlock->RemoveSubblock(this);
}

IGpuMemorySubblock::TSize IGpuMemorySubblock::GetAllocatedSize() const {
	return reservedSize;
}

IGpuMemorySubblock::TSize IGpuMemorySubblock::GetOffsetFromBlock() const {
	return totalOffsetFromBlock;
}

IGpuMemoryBlock* IGpuMemorySubblock::GetOwnerBlock() const {
	return ownerBlock;
}

void IGpuMemorySubblock::SetCursor(TSize position) {
	cursor = position;
}

void IGpuMemorySubblock::ResetCursor() {
	cursor = 0;
}
