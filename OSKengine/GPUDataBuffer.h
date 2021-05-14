#pragma once

#include <vulkan/vulkan.h>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	namespace VULKAN {
		class VulkanImageGen;
	}

	/// <summary>
	/// Buffer que almacena información en la GPU.
	/// </summary>
	struct OSKAPI_CALL GPUDataBuffer {

		friend class RenderAPI;
		friend class ContentManager;
		friend class ModelData;
		friend class ShadowMap;
		friend class SpriteContainer;
		friend class DescriptorSet;
		friend class VULKAN::VulkanImageGen;
		friend class Model;
		friend class RenderizableScene;
		friend class RenderSystem3D;

	public:

		/// <summary>
		/// Crea un buffer vacío.
		/// Debe llamarse manualmente a Free().
		/// </summary>
		GPUDataBuffer() {}

		/// <summary>
		/// Destruye el buffer.
		/// Debe llamarse a Free().
		/// </summary>
		~GPUDataBuffer();

		/// <summary>
		/// Reserva memoria con el tamaño dado.
		/// Debe llamarse a Free antes, si el buffere ya estaba inicializado.
		/// </summary>
		/// <param name="size">Tamaño en bytes.</param>
		void Allocate(size_t size);

		/// <summary>
		/// Mapea toda la memoria, para después poder escribirla.
		/// </summary>
		void MapMemory();
				
		/// <summary>
		/// Mapea una región de memoria, para después poder escribirla.
		/// </summary>
		/// <param name="size">Tamaño de la región )en bytes).</param>
		/// <param name="offset">Offset desde el principio del buffer.</param>
		void MapMemory(size_t size, size_t offset);

		/// <summary>
		/// Mapea, escribe información y desmapea.
		/// </summary>
		/// <param name="data">Información a escribir.</param>
		/// <param name="size">Tamaño de la información.</param>
		/// <param name="offset">Offset sobre el que se va a escribir, respecto al principio del buffer.</param>
		void Write(const void* data, size_t size, size_t offset = 0);

		/// <summary>
		/// Una vez que la memoria ha sido mapeada, escribe información.
		/// Debe llamarse antes a MapMemory().
		/// </summary>
		/// <param name="data">Información a escribir.</param>
		/// <param name="size">Tamaño de la información.</param>
		/// <param name="offset">Offset sobre el que se va a escribir, respecto al principio del buffer.</param>
		void WriteMapped(const void* data, size_t size, size_t offset = 0);

		/// <summary>
		/// Desmapea la memoria.
		/// </summary>
		void UnmapMemory();

		/// <summary>
		/// Libera la memoria.
		/// </summary>
		void Free();

		/// <summary>
		/// Tamaño de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		void SetDynamicUboStructureSize(size_t size);

		/// <summary>
		/// Tamaño del buffer.
		/// </summary>
		size_t GetSize();

		/// <summary>
		/// Tamaño de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		size_t GetDynamicUboStructureSize();

	private:

		/// <summary>
		/// Crea un buffer vacío.
		/// </summary>
		/// <param name="device">Logical device del renderizador.</param>
		/// <param name="usage">Uso que se le va a dar.</param>
		/// <param name="properties">Propiedades necesarias.</param>
		/// <param name="gpuProps">Propiedades de la GPU.</param>
		void Create(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps);

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

		/// <summary>
		/// Uso que se le va a dar.
		/// </summary>
		VkBufferUsageFlags usage = VK_NULL_HANDLE;

		/// <summary>
		/// Propiedades necesarias.
		/// </summary>
		VkMemoryPropertyFlags properties = 0;

		/// <summary>
		/// Propiedades de la GPU.
		/// </summary>
		VkPhysicalDeviceMemoryProperties gpuProps{};

		/// <summary>
		/// ???
		/// </summary>
		/// <param name="memoryTypeFilter"></param>
		/// <returns></returns>
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter);

		/// <summary>
		/// Memoria mapeada en la RAM.
		/// </summary>
		void* mappedData = nullptr;

		/// <summary>
		/// True si la memoria está actualmente mapeada.
		/// </summary>
		bool isMapped = false;

		/// <summary>
		/// Tamaño del buffer.
		/// </summary>
		size_t size = 0;

		/// <summary>
		/// Tamaño de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		size_t dynamicSize = 0;

		//Buffer.

		/// <summary>
		/// Buffer nativo de Vulkan.
		/// </summary>
		VkBuffer buffer = VK_NULL_HANDLE;

		/// <summary>
		/// Memoria en la GPU del buffer.
		/// </summary>
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// <summary>
		/// Alineamiento de la memoria.
		/// </summary>
		uint32_t alignment = 0;


	};

}