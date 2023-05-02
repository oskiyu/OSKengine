#include "GpuMemoryAllocatorVk.h"

#include <vulkan/vulkan.h>

#include "Logger.h"

#include "GpuMemoryBlockVk.h"
#include "GpuMemorySubblockVk.h"

#include "GpuImageVk.h"
#include "Format.h"
#include "FormatVk.h"
#include "GpuVk.h"
#include "GpuMemoryTypeVk.h"
#include "GpuMemoryTypes.h"
#include "Vertex.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "CommandListVk.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"

#include "GpuBuffer.h"

#include "BottomLevelAccelerationStructureVk.h"
#include "TopLevelAccelerationStructureVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryAllocatorVk::GpuMemoryAllocatorVk(IGpu* device)
	: IGpuMemoryAllocator(device) {

	minVertexBufferAlignment = 0;
	minIndexBufferAlignment = 0;
	minUniformBufferAlignment = device->As<GpuVk>()->GetInfo().properties.limits.minUniformBufferOffsetAlignment;
	minStorageBufferAlignment = device->As<GpuVk>()->GetInfo().properties.limits.minStorageBufferOffsetAlignment;
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVk::CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVk::CreateNewBufferBlock(size, device, sharedType, usage).GetPointer();
}

OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVk::CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVk::CreateNewImageBlock(image, device, sharedType, usage).GetPointer();
}

OwnedPtr<GpuImage> GpuMemoryAllocatorVk::CreateImage(const GpuImageCreateInfo& info) {
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

	GpuImageVk* output = new GpuImageVk(
		info.resolution, 
		info.dimension, 
		info.usage, 
		info.numLayers, 
		info.format, 
		info.msaaSamples, 
		info.samplerDesc);

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
	auto block = GpuMemoryBlockVk::CreateNewImageBlock(output, device, info.memoryType, info.usage);
	output->SetBlock(block.GetPointer());

	imageMemoryBlocks.Insert(block.GetPointer());

	// ------ IMAGE ---------- //
	output->CreateVkSampler(info.samplerDesc);

	return output;
}


OwnedPtr<IBottomLevelAccelerationStructure> GpuMemoryAllocatorVk::_CreateBottomAccelerationStructure() {
	return new BottomLevelAccelerationStructureVk();
}

OwnedPtr<ITopLevelAccelerationStructure> GpuMemoryAllocatorVk::_CreateTopAccelerationStructure() {
	return new TopLevelAccelerationStructureVk();
}
