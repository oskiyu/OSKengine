#include "GpuMemoryAllocatorVulkan.h"

#include "GpuMemoryBlockVulkan.h"
#include "GpuMemorySubblockVulkan.h"
#include "GpuImageVulkan.h"
#include "Format.h"
#include <vulkan/vulkan.h>
#include "FormatVulkan.h"
#include "GpuVulkan.h"
#include "GpuMemoryTypeVulkan.h"
#include "GpuMemoryTypes.h"
#include "Vertex.h"
#include "IGpuDataBuffer.h"
#include "GpuVertexBufferVulkan.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "CommandListVulkan.h"
#include "GpuIndexBufferVulkan.h"
#include "GpuUniformBufferVulkan.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "BottomLevelAccelerationStructureVulkan.h"
#include "TopLevelAccelerationStructureVulkan.h"
#include "GpuMemoryTypes.h"
#include "GpuStorageBufferVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryAllocatorVulkan::GpuMemoryAllocatorVulkan(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVulkan::CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVulkan::CreateNewBufferBlock(size, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVulkan::CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVulkan::CreateNewImageBlock(image, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorVulkan::CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) {
	const TSize bufferSize = numVertices * vertexSize;
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::VERTEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, bufferSize);
	stagingBuffer->Unmap();

	GpuVertexBufferVulkan* output = new GpuVertexBufferVulkan(block->GetNextMemorySubblock(bufferSize, 0), bufferSize, 0, numVertices, vertexInfo);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListVulkan>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBuffer(stagingBuffer, output, bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<IGpuIndexBuffer> GpuMemoryAllocatorVulkan::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::INDEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuIndexBufferVulkan* output = new GpuIndexBufferVulkan(block->GetNextMemorySubblock(bufferSize, 0), bufferSize, 0, indices.GetSize());

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListVulkan>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBuffer(stagingBuffer, output, bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<IGpuUniformBuffer> GpuMemoryAllocatorVulkan::CreateUniformBuffer(TSize size) {
	auto block = GetNextBufferMemoryBlock(size, GpuBufferUsage::UNIFORM_BUFFER, GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuUniformBufferVulkan(block->GetNextMemorySubblock(size, device->As<GpuVulkan>()->GetInfo().properties.limits.minUniformBufferOffsetAlignment), size, 0);;
}

OwnedPtr<IGpuStorageBuffer> GpuMemoryAllocatorVulkan::CreateStorageBuffer(TSize size) {
	auto block = GetNextBufferMemoryBlock(size, GpuBufferUsage::STORAGE_BUFFER, GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuStorageBufferVk(block->GetNextMemorySubblock(size, device->As<GpuVulkan>()->GetInfo().properties.limits.minUniformBufferOffsetAlignment), size, 0);
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorVulkan::CreateStagingBuffer(TSize size) {
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size,
		GpuBufferUsage::TRANSFER_SOURCE,
		GpuSharedMemoryType::GPU_AND_CPU)->GetNextMemorySubblock(size, device->As<GpuVulkan>()->GetInfo().properties.limits.minStorageBufferOffsetAlignment), size, 0);
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorVulkan::CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size, alignment), size, 0);
}

OwnedPtr<GpuImage> GpuMemoryAllocatorVulkan::CreateImage(const GpuImageCreateInfo& info) {
	TSize numBytes = GetFormatNumberOfBytes(info.format);

	switch (info.dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: numBytes *= info.resolution.X; break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: numBytes *= info.resolution.X * info.resolution.Y; break;
		case OSK::GRAPHICS::GpuImageDimension::d3D: numBytes *= info.resolution.X * info.resolution.Y * info.resolution.Z; break;
	}

	Vector3ui finalImageSize = info.resolution;

	switch (info.dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: finalImageSize = { info.resolution.X , 1, 1 }; break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: finalImageSize = { info.resolution.X , info.resolution.Y, 1 }; break;
	}

	if (finalImageSize.X == 0)
		finalImageSize.X = 1;
	if (finalImageSize.Y == 0)
		finalImageSize.Y = 1;

	VkSampler sampler = VK_NULL_HANDLE;

	GpuImageVulkan* output = new GpuImageVulkan(info.resolution, info.dimension, info.usage, info.numLayers, info.format, info.msaaSamples, info.samplerDesc);

	TSize numMipLevels = 0;
	switch (info.samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		numMipLevels = output->GetMipLevels();
		break;
	case GpuImageMipmapMode::CUSTOM:
		numMipLevels = info.samplerDesc.maxMipLevel;
		break;
	case GpuImageMipmapMode::NONE:
		numMipLevels = 1;
		break;
	}

	output->CreateVkImage();
	auto block = GpuMemoryBlockVulkan::CreateNewImageBlock(output, device, info.memoryType, info.usage);
	output->SetBlock(block.GetPointer());

	imageMemoryBlocks.Insert(block.GetPointer());

	// ------ IMAGE ---------- //
	output->CreateVkSampler(info.samplerDesc);

	return output;
}

OwnedPtr<ITopLevelAccelerationStructure> GpuMemoryAllocatorVulkan::CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) {
	OwnedPtr<ITopLevelAccelerationStructure> output = new TopLevelAccelerationStructureVulkan();

	for (auto i : bottomStructures)
		output->AddBottomLevelAccelerationStructure(i->As<BottomLevelAccelerationStructureVulkan>());

	output->Setup();

	return output;
}

OwnedPtr<IBottomLevelAccelerationStructure> GpuMemoryAllocatorVulkan::CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) {
	BottomLevelAccelerationStructureVulkan* output = new BottomLevelAccelerationStructureVulkan();
	output->Setup(vertexBuffer, indexBuffer, RtAccelerationStructureFlags::FAST_TRACE); /// @todo
	output->SetMatrix(glm::mat4(1.0f));

	return output;
}
