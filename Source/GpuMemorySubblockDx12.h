#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IGpuMemorySubblock.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemorySubblockDx12 : public IGpuMemorySubblock {

	public:

		GpuMemorySubblockDx12(IGpuMemoryBlock* owner, USize64 size, USize64 offset, ComPtr<ID3D12Resource> resource);

		ID3D12Resource* GetResource() const;

	private:

		ComPtr<ID3D12Resource> resource;

	};

}

#endif
