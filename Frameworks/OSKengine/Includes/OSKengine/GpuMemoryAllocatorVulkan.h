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

	class OSKAPI_CALL GpuMemoryAllocatorVulkan : public IGpuMemoryAllocator {

	public:

		GpuMemoryAllocatorVulkan(IGpu* device);

		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices) override;
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& vertices) override;
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) override;
		OwnedPtr<GpuImage> CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) override;
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) override;

	protected:

		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) override;

	};

}
