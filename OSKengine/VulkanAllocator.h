#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <list>

#include "VulkanMemoryBlock.h"

namespace OSK {

	class GpuDataBuffer;

}

/// <summary>
/// Convierte un número de megabytes en bytes.
/// Uso: x OSK_MB.
/// </summary>
#define OSK_MB * 1000000

namespace OSK::VULKAN {

	/// <summary>
	/// Tamaño mínimo de cada bloque de memoria reservado en la GPU.
	/// </summary>
	constexpr size_t SIZE_OF_GPU_MEMORY_BLOCK = 32 OSK_MB;


	/// <summary>
	/// Clase que maneja la memoria de la GPU.
	/// Funciona asignando grandes bloques de memoria.
	/// A cada buffer le corresponde una parte de un bloque de memoria.
	/// </summary>
	class OSKAPI_CALL MemoryAllocator {

	public:

		/// <summary>
		/// Inicializa las propiedades del asignador.
		/// </summary>
		void Init(VkDevice device, VkPhysicalDeviceMemoryProperties gpuMemoryProperties);

		/// <summary>
		/// Asigna memoria a un buffer.
		/// </summary>
		/// <param name="buffer">Buffer.</param>
		/// <param name="size">Tamaño deseado del buffer.</param>
		/// <param name="usage">Uso que se le va a dar al buffer.</param>
		/// <param name="properties">Propiedades de la memoria necesaria.</param>
		void Allocate(GpuDataBuffer* buffer, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		/// <summary>
		/// Libera toda la memoria usada.
		/// </summary>
		void Free();

		/// <summary>
		/// Estadísticas generales.
		/// </summary>
		MemoryAllocatorStats_t stats;

	private:

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

		/// <summary>
		/// Propiedades de la memoria de la GPU.
		/// </summary>
		VkPhysicalDeviceMemoryProperties gpuMemoryProperties = {};

		/// <summary>
		/// Bloques de memoria asignados.
		/// </summary>
		std::list<MemoryBlock*> blocks;

	};
}