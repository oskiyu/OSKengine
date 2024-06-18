#include "IGpuMemorySubblock.h"

#include "IGpuMemoryBlock.h"
#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuMemorySubblock::IGpuMemorySubblock(IGpuMemoryBlock* block, USize64 size, USize64 offset)
	: ownerBlock(block), reservedSize(size), totalOffsetFromBlock(offset) {
	
}

IGpuMemorySubblock::~IGpuMemorySubblock() {
	ownerBlock->RemoveSubblock(this);
}

USize64 IGpuMemorySubblock::GetAllocatedSize() const {
	return reservedSize;
}

USize64 IGpuMemorySubblock::GetOffsetFromBlock() const {
	return totalOffsetFromBlock;
}

IGpuMemoryBlock* IGpuMemorySubblock::GetOwnerBlock() const {
	return ownerBlock;
}

void IGpuMemorySubblock::SetCursor(UIndex64 position) {
	cursor = position;
}

void IGpuMemorySubblock::ResetCursor() {
	cursor = 0;
}

UIndex64 IGpuMemorySubblock::GetCursor() const {
	return cursor;
}
