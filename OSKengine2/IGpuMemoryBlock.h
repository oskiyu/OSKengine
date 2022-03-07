#pragma once

#include "OSKmacros.h"
#include "LinkedList.hpp"
#include "OwnedPtr.h"

namespace OSK::GRAPHICS {

	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuSharedMemoryType;
	enum class GpuMemoryUsage;

	/// <summary>
	/// Un bloque de memoria representa una región de memoria que
	/// ha sidon reservada.
	/// 
	/// El bloque tiene una lista de subbloques, cada uno representando
	/// una subregión de memoria que almacena un recurso.
	/// </summary>
	class OSKAPI_CALL IGpuMemoryBlock {

	public:

		typedef unsigned int TSize;

		virtual ~IGpuMemoryBlock() = default;

		/// <summary>
		/// Libera la memoria.
		/// </summary>
		virtual void Free() = 0;

		/// <summary>
		/// Marca uno de los subbloques como no utilizado.
		/// </summary>
		void RemoveSubblock(IGpuMemorySubblock* subblock);

		/// <summary>
		/// Obtiene un subbloque con el tamaño dado,
		/// ya sea nuevo o reutilizado.
		/// </summary>
		IGpuMemorySubblock* GetNextMemorySubblock(TSize size);

		/// <summary>
		/// Devuelve el tamaño del bloque.
		/// </summary>
		TSize GetAllocatedSize() const;

		/// <summary>
		/// Devuelve la cantidad de memoria disponible que nunca ha sido
		/// reclamada.
		/// </summary>
		TSize GetAvailableSpace() const;

		GpuSharedMemoryType GetShareType() const;
		GpuMemoryUsage GetUsageType() const;

		IGpu* GetDevice() const;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuMemoryBlock, T> {
			return (T*)this;
		}

	protected:

		/// <summary>
		/// Reserva un bloque de memoria con el tamaño dado para un buffer.
		/// </summary>
		IGpuMemoryBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage);

		virtual OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) = 0;

		/// <summary>
		/// Estructura que se usa cuando un subbloque ya no está en uso:
		/// una región de memoria entre dos subbloques en uso, pero que
		/// puede ser utilizada.
		/// </summary>
		struct ReusableMemorySubblock {
			TSize size;
			TSize offset;

			bool operator==(const ReusableMemorySubblock& other) const;
		};

		LinkedList<IGpuMemorySubblock*> subblocks;
		LinkedList<ReusableMemorySubblock> reusableSubblocks;

		TSize totalSize = 0;
		TSize availableSpace = 0;
		TSize currentOffset = 0;

		GpuSharedMemoryType type;
		GpuMemoryUsage usage;

		IGpu* device = nullptr;

	};

}
