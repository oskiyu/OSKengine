#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

#include "MutexHolder.h"

namespace OSK::GRAPHICS {

	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuSharedMemoryType;
	enum class GpuMemoryUsage;
	enum class Format;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	class GpuImage;

	struct GpuMemoryMappedRange {
		USize64 size = 0;
		USize64 offset = 0;

		bool IsEmpty() const {
			return size == 0;
		}

	};


	/// @brief  Un bloque de memoria representa una región de memoria que
	/// ha sido reservada.
	/// 
	/// El bloque tiene una lista de subbloques, cada uno representando
	/// una subregión de memoria que almacena un recurso.
	class OSKAPI_CALL IGpuMemoryBlock {

	public:

		virtual ~IGpuMemoryBlock() = default;

		OSK_DEFINE_AS(IGpuMemoryBlock);


		/// @brief 
		/// @param subblock 
		/// @threadsafe
		void RemoveSubblock(IGpuMemorySubblock* subblock);

		
		/// @brief Obtiene un subbloque con las características dadas.
		/// @param size Tamaño de memoria, en bytes.
		/// @param alignment Alineamiento de la memoria necesario.
		/// @return Subbloque con las características dadas.
		/// 
		/// @threadsafe
		/// 
		/// @throws GpuMemoryBlockNotEnoughSpaceException si no se logra obtener un bloque con
		/// las características dadas. Esto puede ocurrir aunque en principio el bloque
		/// tenga suficiente memoria debido a fragmentación.
		UniquePtr<IGpuMemorySubblock> GetNextMemorySubblock(USize64 size, USize64 alignment);

		
		/// @brief Devuelve el tamaño total del bloque, ya esté siendo usado o no.
		/// @return Tamaño del bloque, en bytes.
		/// @threadsafe
		USize64 GetAllocatedSize() const;
				
		/// @brief Devuelve la cantidad de memoria disponible que nunca ha sido
		/// reclamada.
		/// No tiene en cuenta los subbloques reutilizables.
		/// @return Tamaño libre no usado previamente, en bytes.
		/// 
		/// @threadsafe
		USize64 GetAvailableSpace() const;

		/// @brief Devuelve el tipo de memoria.
		/// @return Tipo de memoria.
		/// @threadsafe
		GpuSharedMemoryType GetShareType() const;

		/// @brief Devuelve el uso que se le da a esta memoria.
		/// @return Uso de la memoria (BUFFER o IMAGEN).
		/// @threadsafe
		GpuMemoryUsage GetUsageType() const;

		/// @brief Devuelve la GPU en la que reside el bloque.
		/// @return Gpu en la que reside el bloque.
		/// @threadsafe
		inline IGpu* GetGpu() const { return device; }

		/// @brief Comporueba si hay alguna parte del bloque en uso.
		/// @return True si no hay ningún subbloque de este bloque en uso.
		inline bool IsUnused() const { return subblocks.IsEmpty(); }


		/// @brief Mapea toda la memoria.
		/// @pre Este buffer debe haber sido creado
		/// con GpuSharedMemoryType::CPU_AND_GPU.
		void MapAll();

		/// @brief Mapea toda la memoria.
		/// @pre Este buffer debe haber sido creado
		/// con GpuSharedMemoryType::CPU_AND_GPU.
		/// @threadsafe
		void UnmapAll();

		/// @brief Mapea un rango de la memoria.
		/// @pre Este buffer debe haber sido creado
		/// con GpuSharedMemoryType::CPU_AND_GPU.
		/// @pre @p offset < tamaño del buffer.
		/// @pre @p size < tamaño del buffer.
		/// @pre @p offset + @p size < tamaño del buffer.
		/// @threadsafe
		void MapRange(USize64 offset, USize64 size);

		/// @return Memoria mapeada.
		/// Desde el inicio del rango mapeado
		/// (no desde el inicio del bloque).
		/// @return Null si no hay memoria mapeada.
		TByte* GetMappedData();
		const TByte* GetMappedData() const;

		/// @return Rango de memoria mapeada.
		GpuMemoryMappedRange GetMappedRange() const;

	protected:

		/// @brief Reserva un bloque de memoria con el tamaño dado.
		/// @param reservedSize Tamaño del bloque.
		/// @param device GPU en el que se almacena.
		/// @param type Tipo de memoria.
		/// @param usage Uso (BUFFER o IMAGEN).
		IGpuMemoryBlock(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage);

		virtual TByte* MapRange_Impl(USize64 offset, USize64 size) = 0;
		virtual void UnmapAll_Impl() = 0;

		/// @brief Crea un nuevo subbloque a partir de la memoria de este bloque.
		/// @param size Tamaño del subbloque, en bytes.
		/// @param offset Offset del subbloque, en bytes, respecto al inicio de este bloque.
		/// @return Nuevo subbloque.
		/// 
		/// @pre La región de memoria definida por offset y size no debe estar en uso.
		virtual UniquePtr<IGpuMemorySubblock> CreateNewMemorySubblock(USize64 size, USize64 offset) = 0;


		/// @brief Estructura que se usa cuando un subbloque ya no está en uso:
		/// una región de memoria entre dos subbloques en uso, pero que
		/// puede ser reutilizada.
		struct ReusableMemorySubblock {
			USize64 size;
			USize64 offset;

			bool operator==(const ReusableMemorySubblock& other) const;
		};


		/// @brief Referencia a los subbloques activos (en uso).
		/// Sirve para poder destruirlos al destruir el bloque.
		DynamicArray<IGpuMemorySubblock*> subblocks;

		/// @brief Lista con los subbloques reutilizables.
		DynamicArray<ReusableMemorySubblock> reusableSubblocks;


		/// @brief Tamaño total del bloque.
		USize64 totalSize = 0;
		/// @brief Espacio que aún no ha sido usado.
		USize64 availableSpace = 0;
		/// @brief Offset para el próximo subbloque de la zona aún no usada.
		USize64 currentOffset = 0;

		GpuSharedMemoryType type;
		GpuMemoryUsage usage;

		/// @brief GPU donde reside este bloque.
		IGpu* device = nullptr;

		/// @brief Mutex para la búsqueda y alojamiento de
		/// subbloques.
		mutable MutexHolder m_subblockSearchMutex;


		// --- MAPEO DE MEMORIA --- //

		/// @brief Mutex para el mapeo y/o
		/// desmapeo de memoria.
		MutexHolder m_mapMutex;

		/// @brief Memoria mapeada.
		/// Si no está mapeada, será
		/// nullptr.
		TByte* m_mappedData = nullptr;

		/// @brief Región de memoria mapeada.
		GpuMemoryMappedRange m_mappedRange{};

	};

}
