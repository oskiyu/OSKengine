#include "GpuIndexBufferDx12.h"

#include "GpuMemorySubblockDx12.h"
#include "Vertex.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuIndexBufferDx12::GpuIndexBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices)
	: IGpuIndexBuffer(buffer, size, alignment, numIndices) {

}

void GpuIndexBufferDx12::SetView(TSize numberOfIndices) {
	view.BufferLocation = buffer->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = sizeof(TIndexSize) * numberOfIndices;
}

const D3D12_INDEX_BUFFER_VIEW* GpuIndexBufferDx12::GetView() const {
	return &view;
}
