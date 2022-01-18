#include "GpuVertexBufferDx12.h"

#include "GpuMemorySubblockDx12.h"

using namespace OSK;

GpuVertexBufferDx12::GpuVertexBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuVertexBuffer(buffer, size, alignment) {

}

void GpuVertexBufferDx12::SetView(TSize vertexSize, TSize numberOfVertices) {
	view.BufferLocation = buffer->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress();
	view.SizeInBytes = vertexSize * numberOfVertices;
	view.StrideInBytes = vertexSize;
}

D3D12_VERTEX_BUFFER_VIEW GpuVertexBufferDx12::GetView() const {
	return view;
}
