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
#include "GpuImageUsage.h"
#include "FormatDx12.h"
#include "GpuImageDimensions.h"
#include "Math.h"
#include "GpuStorageBufferDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryAllocatorDx12::GpuMemoryAllocatorDx12(IGpu* device)
	: IGpuMemoryAllocator(device) { 
	
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockDx12::CreateNewBufferBlock(size, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorDx12::CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockDx12::CreateNewImageBlock(image, device, sharedType, usage, image->GetNumLayers()).GetPointer();
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorDx12::CreateStagingBuffer(TSize size) {
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size, 
		GpuBufferUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_AND_CPU)->GetNextMemorySubblock(size, 65536), size, 0);
}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorDx12::CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) {
	const TSize bufferSize = numVertices * vertexSize;
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::VERTEX_BUFFER, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, bufferSize);
	stagingBuffer->Unmap();

	GpuVertexBufferDx12* output = new GpuVertexBufferDx12(block->GetNextMemorySubblock(bufferSize, 0), bufferSize, 0, numVertices, vertexInfo);

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

	output->SetView(vertexSize, numVertices);

	return output;
}

OwnedPtr<IGpuIndexBuffer> GpuMemoryAllocatorDx12::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::INDEX_BUFFER, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuIndexBufferDx12* output = new GpuIndexBufferDx12(block->GetNextMemorySubblock(bufferSize, 0), bufferSize, 0, indices.GetSize());

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

	return new GpuUniformBufferDx12(block->GetNextMemorySubblock(size, 0), size, 0);
}

OwnedPtr<IGpuStorageBuffer> OSK::GRAPHICS::GpuMemoryAllocatorDx12::CreateStorageBuffer(TSize size) {
	auto block = GetNextBufferMemoryBlock(size, GpuBufferUsage::STORAGE_BUFFER, GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuStorageBufferDx12(block->GetNextMemorySubblock(size, 0), size, 0);
}

OwnedPtr<GpuImage> GpuMemoryAllocatorDx12::CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) {
	auto output = new GpuImageDx12(size, dimension, usage, numLayers, format, msaaSamples, samplerDesc);
	output->_SetPhysicalSize({
		MATH::PrimerMultiploSuperior<uint32_t>(size.X, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT),
		size.Y,
		size.Z
		});

	auto block = GpuMemoryBlockDx12::CreateNewImageBlock(output, device, sharedType, usage, numLayers);

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

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorDx12::CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

OwnedPtr<IBottomLevelAccelerationStructure> GpuMemoryAllocatorDx12::CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

OwnedPtr<ITopLevelAccelerationStructure> GpuMemoryAllocatorDx12::CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

DescriptorDx12 GpuMemoryAllocatorDx12::GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	auto& list = descriptorBlocks.Get(type);

	if (list.Peek().IsFull())
		list.Insert(DescriptorBlockDx12(type, 256, device->As<GpuDx12>()));
		
	return list.Peek().CreateDescriptor();
}
