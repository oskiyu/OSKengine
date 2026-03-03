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

void IGpuMemorySubblock::MapMemory() {
	MapMemory(reservedSize, 0);
}

void IGpuMemorySubblock::MapMemory(USize64 size, USize64 offset) {
	const auto range = GetOwnerBlock()->GetMappedRange();

	const bool isInsideRange =
		range.offset <= totalOffsetFromBlock + offset &&
		range.offset + range.size >= totalOffsetFromBlock + offset + size;

	if (!isInsideRange) {
		GetOwnerBlock()->MapRange(totalOffsetFromBlock + offset, size);
	}

	SetCursor(offset);
}

void IGpuMemorySubblock::Unmap() {
	const auto range = GetOwnerBlock()->GetMappedRange();

	if (range.offset == totalOffsetFromBlock && range.size == reservedSize) {
		GetOwnerBlock()->UnmapAll();
	}
}

void IGpuMemorySubblock::Write(const void* data, USize64 size) {
	WriteOffset(data, size, cursor);
	cursor += size;
}

void IGpuMemorySubblock::WriteOffset(const void* data, USize64 size, USize64 offset) {
	const auto range = GetOwnerBlock()->GetMappedRange();

	const auto offsetInBlock = totalOffsetFromBlock + offset;
	const auto offsetInRange = offsetInBlock - range.offset;

	std::memcpy(&GetOwnerBlock()->GetMappedData()[offsetInRange], data, size);
}

USize64 IGpuMemorySubblock::GetAllocatedSize() const {
	return reservedSize;
}

USize64 IGpuMemorySubblock::GetOffsetFromBlock() const {
	return totalOffsetFromBlock;
}

IGpuMemoryBlock* IGpuMemorySubblock::GetOwnerBlock() {
	return ownerBlock;
}

const IGpuMemoryBlock* IGpuMemorySubblock::GetOwnerBlock() const {
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

TByte* IGpuMemorySubblock::GetMappedData() {
	TByte* output = GetOwnerBlock()->GetMappedData();

	if (!output) {
		return nullptr;
	}

	return &output[totalOffsetFromBlock];
}

const TByte* IGpuMemorySubblock::GetMappedData() const {
	const TByte* output = GetOwnerBlock()->GetMappedData();

	if (!output) {
		return nullptr;
	}

	return &output[totalOffsetFromBlock];
}
