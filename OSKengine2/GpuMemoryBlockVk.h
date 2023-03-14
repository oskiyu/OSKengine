#pragma once

#include "IGpuMemoryBlock.h"
#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class Format;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	class GpuImage;
	class GpuVk;

	class OSKAPI_CALL GpuMemoryBlockVk : public IGpuMemoryBlock {

	public:

		~GpuMemoryBlockVk();

		static OwnedPtr<GpuMemoryBlockVk> CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		static OwnedPtr<GpuMemoryBlockVk> CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage);

		VkDeviceMemory GetVulkanMemory() const;
		VkBuffer GetVulkanBuffer() const;

	private:

		OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) override;

		GpuMemoryBlockVk(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage);
		GpuMemoryBlockVk(GpuImage*, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type);

		static VkMemoryAllocateFlags GetMemoryAllocateFlags(GpuBufferUsage usage);
		static uint32_t GetMemoryType(uint32_t memoryTypeFilter, GpuVk* device, GpuSharedMemoryType type);

		
		/// @brief Memoria del bloque.
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// @brief Buffer del bloque (puede no ser usado).
		VkBuffer buffer = VK_NULL_HANDLE;

	};

}
