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

	output->SetView(sizeof(Vertex3D), vertices.GetSize());

	return output;
}

OwnedPtr<GpuImage> GpuMemoryAllocatorDx12::CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	auto output = new GpuImageDx12(sizeX, sizeY, format);

	auto block = GpuMemoryBlockDx12::CreateNewImageBlock(output, device, sharedType, usage);
	//auto subblock = block->GetNextMemorySubblock(block->GetAllocatedSize());

	output->SetBlock(block.GetPointer());	
	output->SetResource(output->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource());

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

		for (auto i : list)
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
