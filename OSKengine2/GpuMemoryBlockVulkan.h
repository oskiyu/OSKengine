#pragma once

#include "IGpuMemoryBlock.h"

struct VkDeviceMemory_T;
typedef VkDeviceMemory_T* VkDeviceMemory;
struct VkBuffer_T;
typedef VkBuffer_T* VkBuffer;
struct VkImage_T;
typedef VkImage_T* VkImage;

namespace OSK {

	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	enum class Format;
	class GpuImage;

	class OSKAPI_CALL GpuMemoryBlockVulkan : public IGpuMemoryBlock {

	public:

		~GpuMemoryBlockVulkan();
		void Free() override;

		static OwnedPtr<GpuMemoryBlockVulkan> CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		static OwnedPtr<GpuMemoryBlockVulkan> CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage);

		VkDeviceMemory GetVulkanMemory() const;
		VkBuffer GetVulkanBuffer() const;

	private:

		OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) override;

		GpuMemoryBlockVulkan(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		GpuMemoryBlockVulkan(GpuImage*, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type);

		VkDeviceMemory memory = nullptr;
		VkBuffer buffer = nullptr;

	};

}
