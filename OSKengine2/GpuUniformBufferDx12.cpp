#include "GpuUniformBufferDx12.h"

#include "GpuMemorySubblockDx12.h"

using namespace OSK;

GpuUniformBufferDx12::GpuUniformBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment) 
	: IGpuUniformBuffer(buffer, size, alignment) {

}

D3D12_CONSTANT_BUFFER_VIEW_DESC GpuUniformBufferDx12::GetView() const {
	return view;
}

void GpuUniformBufferDx12::SetView() {
	view.BufferLocation = buffer->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress();
	
	// Múltiplo de 256
	view.SizeInBytes = (GetSize() + 256) & 256;
}
