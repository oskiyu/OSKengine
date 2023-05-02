#pragma once

#include "IGpuMemoryBlock.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK::GRAPHICS {

	enum class GpuBufferUsage;
	enum class Format;
	enum class GpuImageUsage;
	class GpuImage;
	class GpuBuffer;

	class OSKAPI_CALL GpuMemoryBlockDx12 : public IGpuMemoryBlock {

	public:
		
		~GpuMemoryBlockDx12() {
			memory->Release();
		}

		static OwnedPtr<GpuMemoryBlockDx12> CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		static OwnedPtr<GpuMemoryBlockDx12> CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage, TSize numLayers);

		ComPtr<ID3D12Heap>& GetMemory() {
			return memory;
		}

	private:

		OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) override;

		GpuMemoryBlockDx12(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		GpuMemoryBlockDx12(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage, TSize numLayers);

		ComPtr<ID3D12Heap> memory;

		D3D12_RESOURCE_DESC resourceDesc{};

		unsigned int sizeX = 0;
		unsigned int sizeY = 0;

		Format format;

		union {
			GpuImage* image = nullptr;
			GpuBuffer* buffer;
		} resource;

	};

}
