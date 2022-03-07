#include "GpuMemoryBlockDx12.h"

#include "GpuDx12.h"
#include "GpuMemoryTypesDx12.h"

#include "GpuMemoryTypes.h"
#include "Format.h"
#include "GpuMemorySubblockDx12.h"
#include "FormatDx12.h"
#include "IGpuImage.h"
#include <d3d12.h>
#include "OSKengine.h"
#include "RendererDx12.h"
#include "GpuDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryBlockDx12::GpuMemoryBlockDx12(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage)
	: IGpuMemoryBlock(image->GetSize().X * image->GetSize().Y * GetFormatNumberOfBytes(image->GetFormat()), device, type, GpuMemoryUsage::IMAGE), sizeX(image->GetSize().X), sizeY(image->GetSize().Y), format(image->GetFormat()) {

	D3D12_RESOURCE_DESC createInfo{};
	createInfo.Width = sizeX;
	createInfo.Height = sizeY;
	createInfo.DepthOrArraySize = 1;
	createInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	createInfo.Alignment = 0;// D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	createInfo.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	createInfo.Flags = D3D12_RESOURCE_FLAG_NONE;
	createInfo.MipLevels = 1;
	createInfo.SampleDesc.Count = 1;
	createInfo.SampleDesc.Quality = 0;
	createInfo.Format = GetFormatDx12(format);
	
	D3D12_RESOURCE_ALLOCATION_INFO allocInfo = Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()->GetResourceAllocationInfo(0, 1, &createInfo);

	D3D12_HEAP_DESC memoryCreateInfo{};
	memoryCreateInfo.SizeInBytes = allocInfo.SizeInBytes;
	memoryCreateInfo.Properties.Type = GetGpuSharedMemoryTypeDx12(type);

	device->As<GpuDx12>()->GetDevice()->CreateHeap(&memoryCreateInfo, IID_PPV_ARGS(&memory));
}

GpuMemoryBlockDx12::GpuMemoryBlockDx12(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) 
	: IGpuMemoryBlock(reservedSize, device, type, GpuMemoryUsage::BUFFER) {
	D3D12_HEAP_DESC memoryCreateInfo{};
	memoryCreateInfo.SizeInBytes = reservedSize;
	memoryCreateInfo.Properties.Type = GetGpuSharedMemoryTypeDx12(type);

	device->As<GpuDx12>()->GetDevice()->CreateHeap(&memoryCreateInfo, IID_PPV_ARGS(&memory));
}

OwnedPtr<GpuMemoryBlockDx12> GpuMemoryBlockDx12::CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) {
	return new GpuMemoryBlockDx12(reservedSize, device, type, bufferUSage);
}

OwnedPtr<GpuMemoryBlockDx12> GpuMemoryBlockDx12::CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return new GpuMemoryBlockDx12(image, device, type, imageUSage);
}

OwnedPtr<IGpuMemorySubblock> GpuMemoryBlockDx12::CreateNewMemorySubblock(TSize size, TSize offset) {
	if (usage == GpuMemoryUsage::BUFFER) {
		ComPtr<ID3D12Resource> resource;
	
		D3D12_RESOURCE_DESC createInfo{};
		createInfo.Width = size;
		createInfo.Height = 1;
		createInfo.DepthOrArraySize = 1;
		createInfo.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		createInfo.Alignment = 0;// D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		createInfo.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		createInfo.Flags = D3D12_RESOURCE_FLAG_NONE;
		createInfo.MipLevels = 1;
		createInfo.SampleDesc.Count = 1;
		createInfo.SampleDesc.Quality = 0;
		createInfo.Format = DXGI_FORMAT_UNKNOWN;

		device->As<GpuDx12>()->GetDevice()->CreatePlacedResource(memory.Get(), offset, &createInfo, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));

		return new GpuMemorySubblockDx12(this, size, offset, resource);
	}
	else if (usage == GpuMemoryUsage::IMAGE) {
		ComPtr<ID3D12Resource> resource;

		D3D12_RESOURCE_DESC createInfo{};
		createInfo.Width = sizeX;
		createInfo.Height = sizeY;
		createInfo.DepthOrArraySize = 1;
		createInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		createInfo.Alignment = 0;// D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		createInfo.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		createInfo.Flags = D3D12_RESOURCE_FLAG_NONE;
		createInfo.MipLevels = 1;
		createInfo.SampleDesc.Count = 1;
		createInfo.SampleDesc.Quality = 0;
		createInfo.Format = GetFormatDx12(format);

		device->As<GpuDx12>()->GetDevice()->CreatePlacedResource(memory.Get(), offset, &createInfo, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));

		return new GpuMemorySubblockDx12(this, size, offset, resource);
	}

	OSK_ASSERT(false, "No se puede crear subbloque para el uso de memoria dado.");
	return nullptr;
}

void GpuMemoryBlockDx12::Free() {
	memory->Release();
}
