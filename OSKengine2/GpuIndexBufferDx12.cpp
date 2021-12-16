#include "GpuIndexBufferDx12.h"

#include "GpuMemorySubblockDx12.h"
#include "Vertex.h"

using namespace OSK;

void GpuIndexBufferDx12::SetView(TSize numberOfIndices) {
	view.BufferLocation = buffer->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = sizeof(TIndexSize) * numberOfIndices;
}

D3D12_INDEX_BUFFER_VIEW GpuIndexBufferDx12::GetView() const {
	return view;
}
