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

	class OSKAPI_CALL GpuMemoryAllocatorVulkan : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorVulkan(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) override;
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& vertices) override;
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) override;
		OwnedPtr<IGpuStorageBuffer> CreateStorageBuffer(TSize size) override;
		OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;
		OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) override;
		OwnedPtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) override;

	protected:

		OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;
		OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
