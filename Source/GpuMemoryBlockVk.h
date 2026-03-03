#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuMemoryBlock.h"
#include <vulkan/vulkan.h>
#include "VulkanTarget.h"

namespace OSK::GRAPHICS {

	enum class Format;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;

	class GpuImage;
	template <VulkanTarget> class GpuVk;

	template <VulkanTarget Target>
	class OSKAPI_CALL GpuMemoryBlockVk final : public IGpuMemoryBlock {

	public:

		~GpuMemoryBlockVk() override;

		/// @brief Crea y aloja un nuevo bloque de memoria para buffers.
		/// @param reservedSize Tamaño reservado, en bytes.
		/// @param device GPU en la que se alojará.
		/// @param type Tipo de memoria.
		/// @param bufferUSage Uso de los buffers.
		/// @return Bloque con las características dadas.
		/// 
		/// @throws GpuBufferCreationException Si no se pudo crear el buffer.
		/// @throws NoCompatibleGpuMemoryException Si no se encuentra memoria compatible.
		/// @throws GpuMemoryAllocException Si no se pudo alojar memoria en la GPU.
		static UniquePtr<GpuMemoryBlockVk> CreateNewBufferBlock(
			USize64 reservedSize,
			IGpu* device, 
			GpuSharedMemoryType type, 
			GpuBufferUsage bufferUSage);

		/// @brief Crea y aloja un nuevo bloque de memoria para una imagen.
		/// @param image Imagen que se alojará en el bloque.
		/// @param device GPU en la que se alojará.
		/// @param type Tipo de memoria.
		/// @param imageUSage Uso de la imagen.
		/// @return Bloque de memoria con las características dadas.
		/// 
		/// @throws NoCompatibleGpuMemoryException Si no se encuentra memoria compatible.
		/// @throws GpuMemoryAllocException Si no se pudo alojar memoria en la GPU.
		static UniquePtr<GpuMemoryBlockVk> CreateNewImageBlock(
			GpuImage* image, 
			IGpu* device, 
			GpuSharedMemoryType type, 
			GpuImageUsage imageUSage);

		VkDeviceMemory GetVulkanMemory() const;
		VkBuffer GetVulkanBuffer() const;

	private:

		UniquePtr<IGpuMemorySubblock> CreateNewMemorySubblock(USize64 size, USize64 offset) override;

		/// @brief Crea un bloque de memoria para buffer.
		/// @see CreateNewBufferBlock.
		/// @throws GpuBufferCreationException Si no se pudo crear el buffer.
		/// @throws NoCompatibleGpuMemoryException Si no se encuentra memoria compatible.
		/// @throws GpuMemoryAllocException Si no se pudo alojar memoria en la GPU.
		GpuMemoryBlockVk(
			USize64 reservedSize, 
			IGpu* device, 
			GpuSharedMemoryType type, 
			GpuBufferUsage bufferUSage);

		/// @brief Crea un bloque de memoria para imágenes.
		/// @see CreateNewImageBlock.
		/// @throws NoCompatibleGpuMemoryException Si no se encuentra memoria compatible.
		/// @throws GpuMemoryAllocException Si no se pudo alojar memoria en la GPU.
		GpuMemoryBlockVk(
			GpuImage*, 
			IGpu* device, 
			GpuImageUsage imageUsage, 
			GpuSharedMemoryType type);

		static VkMemoryAllocateFlags GetMemoryAllocateFlags(GpuBufferUsage usage);

		/// @throws NoCompatibleGpuMemoryException Si no se encontró memoria compatible.
		static uint32_t GetMemoryType(uint32_t memoryTypeFilter, GpuVk<Target>* device, GpuSharedMemoryType type);

		TByte* MapRange_Impl(USize64 offset, USize64 size) override;
		void UnmapAll_Impl() override;

		
		/// @brief Memoria del bloque.
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// @brief Buffer del bloque (puede no ser usado).
		VkBuffer buffer = VK_NULL_HANDLE;

	};

	template class GpuMemoryBlockVk<VulkanTarget::VK_1_0>;
	template class GpuMemoryBlockVk<VulkanTarget::VK_LATEST>;

}

#endif
