#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuMemoryAllocator.h"
#include "Vector3.hpp"

struct VkDeviceMemory_T;
typedef VkDeviceMemory_T* VkDeviceMemory;

struct VkImage_T;
typedef VkImage_T* VkImage;

struct VkSampler_T;
typedef VkSampler_T* VkSampler;

struct VkImageView_T;
typedef VkImageView_T* VkImageView;

enum VkSampleCountFlagBits;
enum VkImageViewType;
enum VkImageType;

namespace OSK::GRAPHICS {

	enum class GpuImageUsage;

	class OSKAPI_CALL GpuMemoryAllocatorVk final : public IGpuMemoryAllocator {

	public:

		explicit GpuMemoryAllocatorVk(IGpu* device);

		OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;

	protected:

		OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(
			USize64 size, 
			GpuBufferUsage usage, 
			GpuSharedMemoryType sharedType) override;

		OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(
			GpuImage* image, 
			GpuImageUsage usage, 
			GpuSharedMemoryType sharedType) override;

		OwnedPtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() override;
		OwnedPtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() override;

	};

}

#endif
