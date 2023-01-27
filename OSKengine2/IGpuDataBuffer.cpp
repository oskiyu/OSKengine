#include "IGpuDataBuffer.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuDataBuffer::GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment) 
	: buffer(buffer.GetPointer()), size(size), alignment(alignment) {

}

GpuDataBuffer::~GpuDataBuffer() {
	buffer->GetOwnerBlock()->RemoveSubblock(buffer);
}

IGpuMemorySubblock* GpuDataBuffer::GetMemorySubblock() const {
	return buffer;
}

IGpuMemoryBlock* GpuDataBuffer::GetMemoryBlock() const {
	return GetMemorySubblock()->GetOwnerBlock();
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

void GpuDataBuffer::SetCursor(TSize position) {
	buffer->SetCursor(position);
}

void GpuDataBuffer::ResetCursor() {
	buffer->ResetCursor();
}
