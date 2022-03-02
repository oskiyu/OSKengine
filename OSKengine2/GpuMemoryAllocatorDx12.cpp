#include "GpuMemoryAllocatorDx12.h"

#include "GpuMemoryBlockDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuImageDx12.h"
#include "GpuVertexBufferDx12.h"
#include "GpuDx12.h"
#include "GpuMemoryTypesDx12.h"
#include "Vertex.h"
#include "GpuMemoryTypes.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "CommandListDx12.h"
#include "GpuIndexBufferDx12.h"
#include "GpuUniformBufferDx12.h"
#include "FormatDx12.h"

using namespace OSK;

GpuMemoryAllocatorDx12::GpuMemoryAllocatorDx12(IGpu* device)
	: IGpuMemoryAllocator(device) { 
	
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorDx12::CreateStagingBuffer(TSize size) {
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size, 
		GpuBufferUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_AND_CPU)->GetNextMemorySubblock(size), size, 0);
}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorDx12::CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) {
	const TSize bufferSize = vertices.GetSize() * sizeof(Vertex3D);
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::VERTEX_BUFFER, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(vertices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuVertexBufferDx12* output = new GpuVertexBufferDx12(block->GetNextMemorySubblock(bufferSize), bufferSize, 0);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListDx12>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();
	
	singleTimeCommandList->ResourceBarrier(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_DEST);

	singleTimeCommandList->GetCommandList()->CopyResource(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		stagingBuffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource());

	singleTimeCommandList->ResourceBarrier(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	singleTimeCommandList->Close();

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	output->SetView(sizeof(Vertex3D), vertices.GetSize());

	return output;
}

OwnedPtr<IGpuIndexBuffer> GpuMemoryAllocatorDx12::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::INDEX_BUFFER, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuIndexBufferDx12* output = new GpuIndexBufferDx12(block->GetNextMemorySubblock(bufferSize), bufferSize, 0);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListDx12>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->ResourceBarrier(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_DEST);

	singleTimeCommandList->GetCommandList()->CopyResource(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		stagingBuffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource());

	singleTimeCommandList->ResourceBarrier(
		output->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_INDEX_BUFFER);

	singleTimeCommandList->Close();

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	output->SetView(indices.GetSize());

	return output;
}

OwnedPtr<IGpuUniformBuffer> GpuMemoryAllocatorDx12::CreateUniformBuffer(TSize size) {
	auto block = GetNextBufferMemoryBlock(size, GpuBufferUsage::UNIFORM_BUFFER, GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuUniformBufferDx12(block->GetNextMemorySubblock(size), size, 0);
}

OwnedPtr<GpuImage> GpuMemoryAllocatorDx12::CreateImage(const Vector2ui& size, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, bool singleSample) {
	auto output = new GpuImageDx12(size.X, size.Y, format);

	auto block = GpuMemoryBlockDx12::CreateNewImageBlock(output, device, sharedType, usage);
	//auto subblock = block->GetNextMemorySubblock(block->GetAllocatedSize());

	output->SetBlock(block.GetPointer());
	output->SetResource(output->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource());

	if (EFTraits::HasFlag(usage, GpuImageUsage::SAMPLED)) {
		ComPtr<ID3D12DescriptorHeap> textureDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NumDescriptors = 1;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		device->As<GpuDx12>()->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&textureDescriptorHeap));

		D3D12_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
		resourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		resourceViewDesc.Format = GetFormatDx12(format);
		resourceViewDesc.Texture2D.MipLevels = 1;
		device->As<GpuDx12>()->GetDevice()->CreateShaderResourceView(output->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource(), &resourceViewDesc,
			textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		output->_SetDescriptorHeap(textureDescriptorHeap);
	}

	return output;
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	TSize bSize = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1000;

	if (size > bSize)
		bSize = size;

	return GpuMemoryBlockDx12::CreateNewBufferBlock(bSize, device, sharedType, usage).GetPointer();
}

IGpuMemoryBlock* GpuMemoryAllocatorDx12::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	auto it = bufferMemoryBlocks.Find({ size, usage, sharedType });

	if (it.IsEmpty()) {
		auto i = CreateNewBufferMemoryBlock(size, usage, sharedType);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i);

		return i.GetPointer();
	}
	else {
		auto& list = it.GetValue().second;

		for (auto& i : list)
			if (i->GetAvailableSpace() >= size)
				return i.GetPointer();

		auto i = CreateNewBufferMemoryBlock(size, usage, sharedType);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i);

		return i.GetPointer();
	}

	return nullptr;
}

/*OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return GpuMemoryBlockDx12::CreateNewImageBlock(image, device, type, imageUSage).GetPointer();
}
*/
