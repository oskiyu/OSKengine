#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "DynamicArray.hpp"

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
	/// de memoria en la GPU, que después podrán ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupará parte de la
	/// memoria del bloque.
	/// </summary>
	class OSKAPI_CALL IGpuMemoryAllocator {

	public:

		using TSize = unsigned int;

		IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator() = default;

		/// <summary>
		/// El tamaño de memoria que, por defecto, tendrá cada bloque de memoria.
		/// 
		/// Un bloque puede tener más tamaño, si se usa para almacenar un
		/// recurso que ocupe más espacio.
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
		virtual OwnedPtr<GpuImage> CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Crea un buffer de vértices con los vértices dados.
		/// </summary>
		virtual OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) = 0;

		virtual OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) = 0;

	protected:

		/// <summary>
		/// Devuelve un bloque con las características dadas.
		/// 
		/// Si no hay ningún bloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre.</param>
		virtual IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un subbloque con las características dadas.
		/// 
		/// Si no hay ningún subbloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre.</param>
		IGpuMemorySubblock* GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// <summary>
		/// Bloques ya reservados.
		/// </summary>
		HashMap<GpuBufferMemoryBlockInfo, LinkedList<OwnedPtr<IGpuMemoryBlock>>> bufferMemoryBlocks;
		LinkedList<OwnedPtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}
