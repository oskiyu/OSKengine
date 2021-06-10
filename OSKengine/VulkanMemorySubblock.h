#pragma once

#include "OSKmacros.h"

#include <vulkan/vulkan.h>

namespace OSK {
	class GpuDataBuffer;
	class RenderAPI;
	class SpriteContainer;
}

namespace OSK::VULKAN {

	struct MemoryBlock;

	/// <summary>
	/// Un subblocque representa una región de memoria que pertenece a un buffer.
	/// Son gestionados por un MemoryBlock.
	/// </summary>
	struct OSKAPI_CALL MemorySubblock {

		friend struct MemoryBlock;
		friend class RenderAPI;
		friend class ContentManager;
		friend class VulkanImageGen;
		friend class SpriteContainer;

		/// <summary>
		/// Libera el buffer.
		/// También libera el Subblock del bloque.
		/// </summary>
		~MemorySubblock();

		/// <summary>
		/// Mapea toda la memoria del subbloque.
		/// </summary>
		void MapMemory();

		/// <summary>
		/// Mapea una zona de memoria específica.
		/// </summary>
		/// <param name="size">Tamaño de la zona.</param>
		/// <param name="offset">Offset del inicio, desde el inicio del subbloque.</param>
		void MapMemory(size_t size, size_t offset);

		/// <summary>
		/// Mapea toda la memoria, escribe datos y desmapea memoria.
		/// </summary>
		/// <param name="data">Datos.</param>
		/// <param name="size">Tamaño de los datos a copiar.</param>
		/// <param name="offset">Offset desde el principio del subbloque.</param>
		void Write(const void* data, size_t size, size_t offset = 0);

		/// <summary>
		/// Escribe datos.
		/// Si la memoria no está mapeada, la mapea.
		/// </summary>
		/// <param name="data">Datos.</param>
		/// <param name="size">Tamaño de los datos a copiar.</param>
		/// <param name="offset">Offset desde el principio del subbloque.</param>
		void WriteMapped(const void* data, size_t size, size_t offset = 0);

		/// <summary>
		/// Desmapea la memoria (si estaba mapeada).
		/// </summary>
		void UnmapMemory();

		/// <summary>
		/// Libera el subbloque.
		/// Avisa al buffer.
		/// </summary>
		void Free();

		/// <summary>
		/// Tamaño del subbloque.
		/// </summary>
		size_t GetSize() const;

		/// <summary>
		/// Memoria del bloque.
		/// </summary>
		VkDeviceMemory GetNativeGpuMemory() const;

		/// <summary>
		/// Buffer nativo del bloque.
		/// </summary>
		VkBuffer GetNativeGpuBuffer() const;

		/// <summary>
		/// El offset desde el bloque.
		/// </summary>
		size_t GetOffset() const;

	private:

		/// <summary>
		/// Logical device del engine.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

		/// <summary>
		/// Datos mapeados.
		/// </summary>
		void* mappedData = nullptr;

		/// <summary>
		/// True si la memoria ahora está mapeada.
		/// </summary>
		bool isMapped = false;

		/// <summary>
		/// Bloque que ha creado este subbloque.
		/// </summary>
		MemoryBlock* ownerBlock = nullptr;

		/// <summary>
		/// Buffer al que pertenece la región de memoria.
		/// Para avisarle de que no tiene memoria cuando se elimine el subbloque.
		/// </summary>
		OSK::GpuDataBuffer* buffer = nullptr;

		/// <summary>
		/// Memoria del bloque.
		/// </summary>
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// <summary>
		/// Buffer nativo del bloque.
		/// </summary>
		VkBuffer vkBuffer = VK_NULL_HANDLE;

		/// <summary>
		/// Offset desde el principio del bloque.
		/// </summary>
		size_t totalOffsetFromBlock = 0;

		/// <summary>
		/// Tamaño en bytes de esta región de memoria.
		/// </summary>
		size_t sizeOfSubblock = 0;

	};

}
