#include "GpuMemorySubblockDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "GpuMemoryBlockDx12.h"

#include "Assert.h"
#include "GpuMemoryExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemorySubblockDx12::GpuMemorySubblockDx12(IGpuMemoryBlock* owner, USize64 size, USize64 offset, ComPtr<ID3D12Resource> resource)
	: IGpuMemorySubblock(owner, size, offset), resource(resource) {

}

ID3D12Resource* GpuMemorySubblockDx12::GetResource() const {
	return resource.Get();
}

#endif
