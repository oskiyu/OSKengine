#pragma once

#include <vulkan/vulkan.h>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "VulkanMemorySubblock.h"

namespace OSK {

	namespace VULKAN {
		class VulkanImageGen;
	}

	/// <summary>
	/// Buffer que almacena informaci�n en la GPU.
	/// </summary>
	class OSKAPI_CALL GpuDataBuffer {

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
		/// Destruye el buffer.
		/// </summary>
		~GpuDataBuffer();

		/// <summary>
		/// Mapea toda la memoria, para despu�s poder escribirla.
		/// </summary>
		void MapMemory();
				
		/// <summary>
		/// Mapea una regi�n de memoria, para despu�s poder escribirla.
		/// </summary>
		/// <param name="size">Tama�o de la regi�n )en bytes).</param>
		/// <param name="offset">Offset desde el principio del buffer.</param>
		void MapMemory(size_t size, size_t offset);

		/// <summary>
		/// Mapea, escribe informaci�n y desmapea.
		/// </summary>
		/// <param name="data">Informaci�n a escribir.</param>
		/// <param name="size">Tama�o de la informaci�n.</param>
		/// <param name="offset">Offset sobre el que se va a escribir, respecto al principio del buffer.</param>
		void Write(const void* data, size_t size, size_t offset = 0);

		/// <summary>
		/// Una vez que la memoria ha sido mapeada, escribe informaci�n.
		/// Debe llamarse antes a MapMemory().
		/// </summary>
		/// <param name="data">Informaci�n a escribir.</param>
		/// <param name="size">Tama�o de la informaci�n.</param>
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
		/// Tama�o de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		void SetDynamicUboStructureSize(size_t size);

		/// <summary>
		/// Tama�o del buffer.
		/// </summary>
		size_t GetSize();

		/// <summary>
		/// Tama�o de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		size_t GetDynamicUboStructureSize();

		/// <summary>
		/// Subbloque de memoria en la GPU correspondiente a este buffer.
		/// </summary>
		VULKAN::MemorySubblock* memorySubblock = nullptr;

	private:

		/// <summary>
		/// Tama�o de cada estructura individual que se almacena, en caso de ser usado como un dynamic UBO.
		/// </summary>
		size_t dynamicSize = 0;

		/// <summary>
		/// Alineamiento de la memoria.
		/// </summary>
		uint32_t alignment = 0;

	};

}
