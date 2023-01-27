#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

namespace OSK::GRAPHICS {

	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuSharedMemoryType;
	enum class GpuMemoryUsage;
	enum class Format;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	class GpuImage;


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
		void RemoveSubblock(IGpuMemorySubblock* subblock);

		
		/// @brief Obtiene un subbloque con las características dadas.
		/// @param size Tamaño de memoria, en bytes.
		/// @param alignment Alineamiento de la memoria necesario.
		/// @return Subbloque con las características dadas.
		/// 
		/// @throws std::runtime_exception si no se logra obtener un bloque con
		/// las características dadas. Esto puede ocurrir aunque en principio el bloque
		/// tenga suficiente memoria debido a fragmentación.
		IGpuMemorySubblock* GetNextMemorySubblock(TSize size, TSize alignment);

		
		/// @brief Devuelve el tamaño total del bloque, ya esté siendo usado o no.
		/// @return Tamaño del bloque, en bytes.
		TSize GetAllocatedSize() const;
				
		/// @brief Devuelve la cantidad de memoria disponible que nunca ha sido
		/// reclamada.
		/// No tiene en cuenta los subbloques reutilizables.
		/// @return Tamaño libre no usado previamente, en bytes.
		TSize GetAvailableSpace() const;

		/// @brief Devuelve el tipo de memoria.
		/// @return Tipo de memoria.
		GpuSharedMemoryType GetShareType() const;

		/// @brief Devuelve el uso que se le da a esta memoria.
		/// @return Uso de la memoria (BUFFER o IMAGEN).
		GpuMemoryUsage GetUsageType() const;

		/// @brief Devuelve la GPU en la que reside el bloque.
		/// @return Gpu en la que reside el bloque.
		inline IGpu* GetGpu() const { return device; }

	protected:

		/// @brief Reserva un bloque de memoria con el tamaño dado.
		/// @param reservedSize Tamaño del bloque.
		/// @param device GPU en el que se almacena.
		/// @param type Tipo de memoria.
		/// @param usage Uso (BUFFER o IMAGEN).
		IGpuMemoryBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage);

		/// @brief Crea un nuevo subbloque a partir de la memoria de este bloque.
		/// @param size Tamaño del subbloque, en bytes.
		/// @param offset Offset del subbloque, en bytes, respecto al inicio de este bloque.
		/// @return Nuevo subbloque.
		/// 
		/// @pre La región de memoria definida por offset y size no debe estar en uso.
		virtual OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(TSize size, TSize offset) = 0;


		/// @brief Estructura que se usa cuando un subbloque ya no está en uso:
		/// una región de memoria entre dos subbloques en uso, pero que
		/// puede ser reutilizada.
		struct ReusableMemorySubblock {
			TSize size;
			TSize offset;

			bool operator==(const ReusableMemorySubblock& other) const;
		};


		/// @brief Referencia a los subbloques activos (en uso).
		/// Sirve para poder destruirlos al destruir el bloque.
		DynamicArray<IGpuMemorySubblock*> subblocks;

		/// @brief Lista con los subbloques reutilizables.
		DynamicArray<ReusableMemorySubblock> reusableSubblocks;


		/// @brief Tamaño total del bloque.
		TSize totalSize = 0;
		/// @brief Espacio que aún no ha sido usado.
		TSize availableSpace = 0;
		/// @brief Offset para el próximo subbloque de la zona aún no usada.
		TSize currentOffset = 0;

		GpuSharedMemoryType type;
		GpuMemoryUsage usage;

		/// @brief GPU donde reside este bloque.
		IGpu* device = nullptr;

	};

}
