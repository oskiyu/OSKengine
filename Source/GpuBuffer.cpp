#include "GpuBuffer.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"

#include "GpuBufferRange.h"


using namespace OSK;
using namespace OSK::GRAPHICS;

GpuBuffer::GpuBuffer(UniquePtr<IGpuMemorySubblock>&& buffer, USize64 size, USize64 alignment, const ICommandQueue* ownerQueue)
	: m_buffer(std::move(buffer)), m_ownerQueue(ownerQueue), m_size(size), m_alignment(alignment) {

}


void GpuBuffer::SetVertexView(const VertexBufferView& view) {
	m_vertexView = view;
}

void GpuBuffer::SetIndexView(const IndexBufferView& view) {
	m_indexView = view;
}

const VertexBufferView& GpuBuffer::GetVertexView() const {
	return m_vertexView.value();
}

const IndexBufferView& GpuBuffer::GetIndexView() const {
	return m_indexView.value();
}

bool GpuBuffer::HasVertexView() const {
	return m_vertexView.has_value();
}

bool GpuBuffer::HasIndexView() const {
	return m_indexView.has_value();
}

const IGpuMemorySubblock* GpuBuffer::GetMemorySubblock() const {
	return m_buffer.GetPointer();
}

IGpuMemorySubblock* GpuBuffer::GetMemorySubblock() {
	return m_buffer.GetPointer();
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
	m_buffer->MapMemory();
}

void GpuBuffer::MapMemory(USize64 size, USize64 offset) {
	m_buffer->MapMemory(size, offset);
}

void GpuBuffer::Write(const void* data, USize64 size) {
	m_buffer->Write(data, size);
}

void GpuBuffer::WriteOffset(const void* data, USize64 size, USize64 offset) {
	m_buffer->WriteOffset(data, size, offset);
}

void GpuBuffer::Unmap() {
	m_buffer->Unmap();
}

USize64 GpuBuffer::GetSize() const {
	return m_size;
}

USize64 GpuBuffer::GetAlignment() const {
	return m_alignment;
}

void GpuBuffer::SetCursor(UIndex64 position) {
	m_buffer->SetCursor(position);
}

void GpuBuffer::ResetCursor() {
	m_buffer->ResetCursor();
}

UIndex64 GpuBuffer::GetCursor() const {
	return m_buffer->GetCursor();
}
