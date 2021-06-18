#pragma once

#include "OSKmacros.h"

#include <vulkan/vulkan.h>
#include <list>
#include <vector>
#include "VulkanMemorySubblock.h"

#include "UniquePtr.hpp"

namespace OSK::VULKAN {

	/// <summary>
	/// Estadísticas de uso del asignador de memoria.
	/// </summary>
	struct MemoryAllocatorStats_t {

		/// <summary>
		/// Bloques de memoria creados.
		/// </summary>
		uint32_t totalMemoryAllocations = 0;

		/// <summary>
		/// Total de memoria reservada por la aplicación.
		/// </summary>
		int64_t totalReservedSize = 0;

		/// <summary>
		/// Memoria usada por la aplicación.
		/// (used <= reserved).
		/// </summary>
		int64_t totalUsedSize = 0;

		/// <summary>
		/// Asignaciones de memoria manejadas por el asignador de memoria.
		/// </summary>
		uint32_t totalSubAllocations = 0;

	};

	/// <summary>
	/// UN bloque de memoria de la GPU.
	/// Puede usarse para asignar partes del bloque a buffers individuales.
	/// </summary>
	struct OSKAPI_CALL MemoryBlock {

		/// <summary>
		/// Crea el bloque.
		/// </summary>
		/// <param name="logicalDevice">Logical device del engine.</param>
		/// <param name="gpuMemoryProperties">Propiedades de la GPU.</param>
		/// <param name="allocatorUsedMemory">Puntero para estadísticas.</param>
		MemoryBlock(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties gpuMemoryProperties, MemoryAllocatorStats_t* allocatorUsedMemory);

		/// <summary>
		/// Destruye el bloque.
		/// Libera la memoria y avisa a los subbloques.
		/// </summary>
		~MemoryBlock();

		/// <summary>
		/// Asigna un bloque de memoria de la GPU.
		/// </summary>
		/// <param name="size">Tamaño del bloque.</param>
		/// <param name="properties">Propiedades de la memoria.</param>
		/// <param name="usage">Uso de la memoria.</param>
		void InitialAlloc(size_t size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);

		/// <summary>
		/// Asigna memoria a un buffer.
		/// </summary>
		/// <param name="buffer">Buffer.</param>
		/// <param name="size">Tamaño que necesita el buffer.</param>
		/// <returns>True si se ha completado la asignación.</returns>
		bool AllocateBuffer(OSK::GpuDataBuffer* buffer, size_t size);

		/// <summary>
		/// Libera la memoria del bloque.
		/// Avisa a los buffers de los subbloques.
		/// </summary>
		void Free();

		/// <summary>
		/// Todos los subbloques activos.
		/// </summary>
		std::list<MemorySubblock*> assignedSubblocks;

		/// <summary>
		/// Propiedades de la memoria del bloque.
		/// </summary>
		VkMemoryPropertyFlags memoryProperties;

		/// <summary>
		/// Uso de la memoria del bloque.
		/// </summary>
		VkBufferUsageFlags usage;

		/// <summary>
		/// Tamaño del bloque.
		/// </summary>
		size_t size;

		/// <summary>
		/// Espacio liber nunca asignado del bloque.
		/// </summary>
		size_t availableSize;

		/// <summary>
		/// Memoria GPU.
		/// </summary>
		VkDeviceMemory memory = nullptr;

		/// <summary>
		/// Buffer de la GPU.
		/// </summary>
		VkBuffer buffer = nullptr;

		/// <summary>
		/// Puntero para estadísticas del asignador.
		/// </summary>
		MemoryAllocatorStats_t* stats = nullptr;

		/// <summary>
		/// Desactiva un subbloque activo.
		/// </summary>
		/// <param name="subblock"></param>
		void RemoveBlock(MemorySubblock* subblock);

	private:

		/// <summary>
		/// Logical device del engine.
		/// </summary>
		VkDevice logicalDevice = nullptr;

		/// <summary>
		/// Propiedades de la emoria de la GPU.
		/// </summary>
		VkPhysicalDeviceMemoryProperties gpuMemoryProperties = {};
		
		/// <summary>
		/// ???
		/// </summary>
		/// <param name="memoryTypeFilter"></param>
		/// <returns></returns>
		uint32_t GetMemoryType(uint32_t memoryTypeFilter);

		/// <summary>
		/// Representa una región de memoria que ha sido usada por un buffer,
		/// pero que ahora está libre.
		/// </summary>
		struct ReusableMemorySubblock {

			/// <summary>
			/// Tamaño de la región de la memoria.
			/// </summary>
			size_t size = 0;

			/// <summary>
			/// Offset de la región de memoria desde el principio del bloque.
			/// </summary>
			size_t offset = 0;

			bool operator==(const ReusableMemorySubblock& other) const {
				return offset == other.offset;
			}
		};

		/// <summary>
		/// Partes del bloque reutilizables, sin dueño actualmente.
		/// </summary>
		std::list<ReusableMemorySubblock> reusableSubblocks;

	};

}
