#pragma once

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

		GpuMemoryAllocatorVk(IGpu* device);

		OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;

	protected:

		OwnedPtr<IGpuVertexBuffer> _CreateVertexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment,
			TSize numVertices,
			const VertexInfo& vertexInfo) override;

		OwnedPtr<IGpuIndexBuffer> _CreateIndexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment,
			TSize numIndices) override;

		OwnedPtr<IGpuUniformBuffer> _CreateUniformBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) override;

		OwnedPtr<IGpuStorageBuffer> _CreateStorageBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) override;


		OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(
			TSize size, 
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
