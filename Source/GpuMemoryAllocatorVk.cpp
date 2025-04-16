#include "GpuMemoryAllocatorVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

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

	LoadDefaultNormalTexture();
}

UniquePtr<IGpuMemoryBlock> GpuMemoryAllocatorVk::CreateNewBufferBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVk::CreateNewBufferBlock(size, device, sharedType, usage);
}

UniquePtr<IGpuMemoryBlock> GpuMemoryAllocatorVk::CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	return GpuMemoryBlockVk::CreateNewImageBlock(image, device, sharedType, usage);
}

UniquePtr<GpuImage> GpuMemoryAllocatorVk::CreateImage(const GpuImageCreateInfo& info) {
	USize64 numBytes = GetFormatNumberOfBytes(info.format);

	switch (info.dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: numBytes *= static_cast<USize64>(info.resolution.x); break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: numBytes *= static_cast<USize64>(info.resolution.x) * static_cast<USize64>(info.resolution.y); break;
		case OSK::GRAPHICS::GpuImageDimension::d3D: numBytes *= static_cast<USize64>(info.resolution.x) * static_cast<USize64>(info.resolution.y) * static_cast<USize64>(info.resolution.z); break;
	}

	Vector3ui finalImageSize = info.resolution;

	switch (info.dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: finalImageSize = { info.resolution.x , 1, 1 }; break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: finalImageSize = { info.resolution.x , info.resolution.y, 1 }; break;
	}

	if (finalImageSize.x == 0)
		finalImageSize.x = 1;
	if (finalImageSize.y == 0)
		finalImageSize.y = 1;

	UniquePtr<GpuImageVk> output = MakeUnique<GpuImageVk>(info, Engine::GetRenderer()->GetOptimalQueue(info.queueType));

	output->CreateVkImage();
	auto block = GpuMemoryBlockVk::CreateNewImageBlock(output.GetPointer(), device, info.memoryType, info.usage);

	imageMemoryBlocks.Insert(block.GetPointer());

	output->SetBlock(std::move(block));

	return output;
}


UniquePtr<IBottomLevelAccelerationStructure> GpuMemoryAllocatorVk::_CreateBottomAccelerationStructure() {
	return MakeUnique<BottomLevelAccelerationStructureVk>();
}

UniquePtr<ITopLevelAccelerationStructure> GpuMemoryAllocatorVk::_CreateTopAccelerationStructure() {
	return MakeUnique<TopLevelAccelerationStructureVk>();
}

#endif
