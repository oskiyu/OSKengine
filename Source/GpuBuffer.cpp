#include "GpuBuffer.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"

#include "GpuBufferRange.h"


using namespace OSK;
using namespace OSK::GRAPHICS;

GpuBuffer::GpuBuffer(OwnedPtr<IGpuMemorySubblock> buffer, USize64 size, USize64 alignment, const ICommandQueue* ownerQueue)
	: buffer(buffer.GetPointer()), m_ownerQueue(ownerQueue), size(size), alignment(alignment) {

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

void GpuBuffer::_UpdateCurrentBarrier(const GpuBarrierInfo& barrier) {
	m_currentBarrier = barrier;
}

const GpuBarrierInfo& GpuBuffer::GetCurrentBarrier() const {
	return m_currentBarrier;
}

const ICommandQueue* GpuBuffer::GetOwnerQueue() const {
	return m_ownerQueue;
}

void GpuBuffer::_UpdateOwnerQueue(const ICommandQueue* ownerQueue) {
	m_ownerQueue = ownerQueue;
}

GpuBufferRange GpuBuffer::GetWholeBufferRange() const {
	GpuBufferRange output{};
	output.offset = GetMemorySubblock()->GetOffsetFromBlock();
	output.size = GetMemorySubblock()->GetAllocatedSize();

	return output;
}

void GpuBuffer::MapMemory() {
	buffer->MapMemory();
}

void GpuBuffer::MapMemory(USize64 size, USize64 offset) {
	buffer->MapMemory(size, offset);
}

void GpuBuffer::Write(const void* data, USize64 size) {
	buffer->Write(data, size);
}

void GpuBuffer::WriteOffset(const void* data, USize64 size, USize64 offset) {
	buffer->WriteOffset(data, size, offset);
}

void GpuBuffer::Unmap() {
	buffer->Unmap();
}

USize64 GpuBuffer::GetSize() const {
	return size;
}

USize64 GpuBuffer::GetAlignment() const {
	return alignment;
}

void GpuBuffer::SetCursor(UIndex64 position) {
	buffer->SetCursor(position);
}

void GpuBuffer::ResetCursor() {
	buffer->ResetCursor();
}

UIndex64 GpuBuffer::GetCursor() const {
	return buffer->GetCursor();
}
