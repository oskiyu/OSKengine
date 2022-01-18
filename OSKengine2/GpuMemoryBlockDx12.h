#pragma once

#include "IGpuMemoryBlock.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK {

	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	enum class Format;
	class GpuImage;

	class OSKAPI_CALL GpuMemoryBlockDx12 : public IGpuMemoryBlock {

	public:
		
		static OwnedPtr<GpuMemoryBlockDx12> CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		static OwnedPtr<GpuMemoryBlockDx12> CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage);

		ComPtr<ID3D12Heap>& GetMemory() {
			return memory;
		}

		void Free() override;

	private:

		OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) override;

		GpuMemoryBlockDx12(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		GpuMemoryBlockDx12(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage);

		ComPtr<ID3D12Heap> memory;

		unsigned int sizeX = 0;
		unsigned int sizeY = 0;

		Format format;

	};

}
