#include "GpuDataBuffer.h"

#include "Memory.h"

using namespace OSK;
using namespace OSK::Memory;


void GpuDataBuffer::MapMemory() {
	memorySubblock->MapMemory();
}

void GpuDataBuffer::MapMemory(size_t size, size_t offset) {
	memorySubblock->MapMemory(size, offset);
}

void GpuDataBuffer::Write(const void* data, size_t size, size_t offset) {
	memorySubblock->Write(data, size, offset);
}

void GpuDataBuffer::WriteMapped(const void* data, size_t size, size_t offset) {
	memorySubblock->WriteMapped(data, size, offset);
}

void GpuDataBuffer::UnmapMemory() {
	memorySubblock->UnmapMemory();
}

void GpuDataBuffer::Free() {
	if (memorySubblock) {
		memorySubblock->Free();
		memorySubblock = nullptr;
	}
}

void GpuDataBuffer::SetDynamicUboStructureSize(size_t size) {
	dynamicSize = size;
}

size_t GpuDataBuffer::GetSize() {
	return memorySubblock->GetSize();
}

size_t GpuDataBuffer::GetDynamicUboStructureSize() {
	return dynamicSize;
}
	
GpuDataBuffer::~GpuDataBuffer() {
	Free();
}
