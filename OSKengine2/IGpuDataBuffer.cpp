#include "IGpuDataBuffer.h"

#include "IGpuMemorySubblock.h"

using namespace OSK;

GpuDataBuffer::GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment) 
	: buffer(buffer), size(size), alignment(alignment) {

}

GpuDataBuffer::~GpuDataBuffer() {
	Free();
}

void GpuDataBuffer::MapMemory() {
	buffer->MapMemory();
}

void GpuDataBuffer::MapMemory(TSize size, TSize offset) {
	buffer->MapMemory(size, offset);
}

void GpuDataBuffer::Write(const void* data, TSize size) {
	buffer->Write(data, size);
}

void GpuDataBuffer::WriteOffset(const void* data, TSize size, TSize offset) {
	buffer->WriteOffset(data, size, offset);
}

void GpuDataBuffer::Unmap() {
	buffer->Unmap();
}

TSize GpuDataBuffer::GetSize() const {
	return size;
}

TSize GpuDataBuffer::GetAlignment() const {
	return alignment;
}

void GpuDataBuffer::Free() {
	buffer.Delete();
}