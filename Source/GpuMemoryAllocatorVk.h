#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuMemoryAllocator.h"
#include "Vector3.hpp"
#include "VulkanTarget.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class GpuImageUsage;

	template <VulkanTarget Target>
	class OSKAPI_CALL GpuMemoryAllocatorVk final : public IGpuMemoryAllocator {

	public:

		explicit GpuMemoryAllocatorVk(IGpu* device);

		UniquePtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;

	protected:

		UniquePtr<IGpuMemoryBlock> CreateNewBufferBlock(
			USize64 size, 
			GpuBufferUsage usage, 
			GpuSharedMemoryType sharedType) override;

		UniquePtr<IGpuMemoryBlock> CreateNewImageBlock(
			GpuImage* image, 
			GpuImageUsage usage, 
			GpuSharedMemoryType sharedType) override;

		UniquePtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() override;
		UniquePtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() override;

	};

	template class GpuMemoryAllocatorVk<VulkanTarget::VK_1_0>;
	template class GpuMemoryAllocatorVk<VulkanTarget::VK_LATEST>;

}

#endif
