#include "GpuMemorySubblockDx12.h"

#include "GpuMemoryBlockDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemorySubblockDx12::GpuMemorySubblockDx12(IGpuMemoryBlock* owner, TSize size, TSize offset, ComPtr<ID3D12Resource> resource)
	: IGpuMemorySubblock(owner, size, offset), resource(resource) {

}

void GpuMemorySubblockDx12::MapMemory() {
	MapMemory(GetAllocatedSize(), 0);
}

void GpuMemorySubblockDx12::MapMemory(TSize size, TSize offset) {
	D3D12_RANGE range{};
	range.Begin = offset;
	range.End = offset + size;
	
	const HRESULT result = resource->Map(0, &range, &mappedData);

	isMapped = true;
}

void GpuMemorySubblockDx12::Write(const void* data, TSize size) {
	WriteOffset(data, size, cursor);
	cursor += size;
}

void GpuMemorySubblockDx12::WriteOffset(const void* data, TSize size, TSize offset) {
	OSK_ASSERT(isMapped, "El buffer no está mapeado.");

	memcpy((char*)mappedData + offset, data, size);
}

void GpuMemorySubblockDx12::Unmap() {
	resource->Unmap(0, nullptr);
}

ID3D12Resource* GpuMemorySubblockDx12::GetResource() const {
	return resource.Get();
}
