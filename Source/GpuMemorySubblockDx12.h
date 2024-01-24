#pragma once

#include "IGpuMemorySubblock.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemorySubblockDx12 : public IGpuMemorySubblock {

	public:

		GpuMemorySubblockDx12(IGpuMemoryBlock* owner, USize64 size, USize64 offset, ComPtr<ID3D12Resource> resource);

		void MapMemory() override;
		void MapMemory(USize64 size, USize64 offset) override;
		void Write(const void* data, USize64 size) override;
		void WriteOffset(const void* data, USize64 size, USize64 offset) override;
		void Unmap() override;

		ID3D12Resource* GetResource() const;

	private:

		ComPtr<ID3D12Resource> resource;

	};

}
