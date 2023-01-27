#pragma once

#include "IGpuMemoryBlock.h"
#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class Format;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	class GpuImage;
	class GpuVulkan;

	class OSKAPI_CALL GpuMemoryBlockVulkan : public IGpuMemoryBlock {

	public:

		~GpuMemoryBlockVulkan();

		static OwnedPtr<GpuMemoryBlockVulkan> CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		static OwnedPtr<GpuMemoryBlockVulkan> CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage);

		VkDeviceMemory GetVulkanMemory() const;
		VkBuffer GetVulkanBuffer() const;

	private:

		OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) override;

		GpuMemoryBlockVulkan(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		GpuMemoryBlockVulkan(GpuImage*, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type);

		static VkMemoryAllocateFlags GetMemoryAllocateFlags(GpuBufferUsage usage);
		static uint32_t GetMemoryType(uint32_t memoryTypeFilter, GpuVulkan* device, GpuSharedMemoryType type);

		
		/// @brief Memoria del bloque.
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// @brief Buffer del bloque (puede no ser usado).
		VkBuffer buffer = VK_NULL_HANDLE;

	};

}
