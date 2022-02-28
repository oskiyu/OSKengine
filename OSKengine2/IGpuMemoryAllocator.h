#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "Vector2.hpp"

namespace OSK {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuSharedMemoryType;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	enum class Format;

	class GpuDataBuffer;
	class GpuImage;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	class IGpuUniformBuffer;
	struct Vertex3D;

	struct OSKAPI_CALL GpuBufferMemoryBlockInfo {
		unsigned int size;
		GpuBufferUsage usage;
		GpuSharedMemoryType sharedType;

		bool operator==(const GpuBufferMemoryBlockInfo& other) const;
	};

	template <> static size_t Hash<GpuBufferMemoryBlockInfo>(const GpuBufferMemoryBlockInfo& elem);

	/// <summary>
	/// El asignador de memoria se encarga de reservar grandes bloques
	/// de memoria en la GPU, que despu�s podr�n ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupar� parte de la
	/// memoria del bloque.
	/// </summary>
	class OSKAPI_CALL IGpuMemoryAllocator {

	public:

		using TSize = unsigned int;

		IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator() = default;

		/// <summary>
		/// El tama�o de memoria que, por defecto, tendr� cada bloque de memoria.
		/// 
		/// Un bloque puede tener m�s tama�o, si se usa para almacenar un
		/// recurso que ocupe m�s espacio.
		/// </summary>
		static TSize SizeOfMemoryBlockInMb;

		/// <summary>
		/// Libera toda la memoria.
		/// Libera todos los bloques.
		/// </summary>
		void Free();

		template <typename T> T* As() const requires std::is_base_of_v<IGpuMemoryAllocator, T> {
			return (T*)this;
		}

		/// <summary>
		/// Crea una nueva imagen en la GPU.
		/// </summary>
		virtual OwnedPtr<GpuImage> CreateImage(const Vector2ui& size, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, bool singleSample) = 0;

		/// <summary>
		/// Crea un buffer de v�rtices con los v�rtices dados.
		/// </summary>
		virtual OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) = 0;

		/// <summary>
		/// Crea un buffer de �ndices con los �ndices dados.
		/// </summary>
		virtual OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) = 0;

		/// <summary>
		/// 
		/// </summary>
		virtual OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un buffer temporal con el tama�o dado.
		/// Este buffer alojar� memoria en una regi�n donde la CPU tiene acceso,
		/// de tal manera que se podr�n enviar datos a esta regi�n, y despu�s
		/// la GPU podr� copiar esta regi�n de memoria a una regi�n de memoria de video
		/// m�s r�pida.
		/// </summary>
		virtual OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) = 0;

	protected:

		/// <summary>
		/// Devuelve un bloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n bloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre.</param>
		virtual IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un subbloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n subbloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre.</param>
		IGpuMemorySubblock* GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// <summary>
		/// Bloques ya reservados.
		/// </summary>
		HashMap<GpuBufferMemoryBlockInfo, LinkedList<OwnedPtr<IGpuMemoryBlock>>> bufferMemoryBlocks;
		LinkedList<OwnedPtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}
