#include "GpuBuffer.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuBuffer::GpuBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment) 
	: buffer(buffer.GetPointer()), size(size), alignment(alignment) {

}

GpuBuffer::~GpuBuffer() {
	buffer->GetOwnerBlock()->RemoveSubblock(buffer);
}


void GpuBuffer::SetVertexView(const VertexBufferView& view) {
	vertexView = view;
}

void GpuBuffer::SetIndexView(const IndexBufferView& view) {
	indexView = view;
}

const VertexBufferView& GpuBuffer::GetVertexView() const {
	return vertexView.value();
}

const IndexBufferView& GpuBuffer::GetIndexView() const {
	return indexView.value();
}

bool GpuBuffer::HasVertexView() const {
	return vertexView.has_value();
}

bool GpuBuffer::HasIndexView() const {
	return indexView.has_value();
}

IGpuMemorySubblock* GpuBuffer::GetMemorySubblock() const {
	return buffer;
}

IGpuMemoryBlock* GpuBuffer::GetMemoryBlock() const {
	return GetMemorySubblock()->GetOwnerBlock();
}

void GpuBuffer::MapMemory() {
	buffer->MapMemory();
}

void GpuBuffer::MapMemory(TSize size, TSize offset) {
	buffer->MapMemory(size, offset);
}

void GpuBuffer::Write(const void* data, TSize size) {
	buffer->Write(data, size);
}

void GpuBuffer::WriteOffset(const void* data, TSize size, TSize offset) {
	buffer->WriteOffset(data, size, offset);
}

void GpuBuffer::Unmap() {
	buffer->Unmap();
}

TSize GpuBuffer::GetSize() const {
	return size;
}

TSize GpuBuffer::GetAlignment() const {
	return alignment;
}

void GpuBuffer::SetCursor(TSize position) {
	buffer->SetCursor(position);
}

void GpuBuffer::ResetCursor() {
	buffer->ResetCursor();
}
