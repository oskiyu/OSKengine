#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "DynamicArray.hpp"
#include "OwnedPtr.h"

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


	/// @brief  Un bloque de memoria representa una regi�n de memoria que
	/// ha sido reservada.
	/// 
	/// El bloque tiene una lista de subbloques, cada uno representando
	/// una subregi�n de memoria que almacena un recurso.
	class OSKAPI_CALL IGpuMemoryBlock {

	public:

		virtual ~IGpuMemoryBlock() = default;

		OSK_DEFINE_AS(IGpuMemoryBlock);


		/// @brief 
		/// @param subblock 
		/// @threadsafe
		void RemoveSubblock(IGpuMemorySubblock* subblock);

		
		/// @brief Obtiene un subbloque con las caracter�sticas dadas.
		/// @param size Tama�o de memoria, en bytes.
		/// @param alignment Alineamiento de la memoria necesario.
		/// @return Subbloque con las caracter�sticas dadas.
		/// 
		/// @threadsafe
		/// 
		/// @throws GpuMemoryBlockNotEnoughSpaceException si no se logra obtener un bloque con
		/// las caracter�sticas dadas. Esto puede ocurrir aunque en principio el bloque
		/// tenga suficiente memoria debido a fragmentaci�n.
		IGpuMemorySubblock* GetNextMemorySubblock(USize64 size, USize64 alignment);

		
		/// @brief Devuelve el tama�o total del bloque, ya est� siendo usado o no.
		/// @return Tama�o del bloque, en bytes.
		/// @threadsafe
		USize64 GetAllocatedSize() const;
				
		/// @brief Devuelve la cantidad de memoria disponible que nunca ha sido
		/// reclamada.
		/// No tiene en cuenta los subbloques reutilizables.
		/// @return Tama�o libre no usado previamente, en bytes.
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
		/// @return True si no hay ning�n subbloque de este bloque en uso.
		inline bool IsUnused() const { return subblocks.IsEmpty(); }

	protected:

		/// @brief Reserva un bloque de memoria con el tama�o dado.
		/// @param reservedSize Tama�o del bloque.
		/// @param device GPU en el que se almacena.
		/// @param type Tipo de memoria.
		/// @param usage Uso (BUFFER o IMAGEN).
		IGpuMemoryBlock(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuMemoryUsage usage);

		/// @brief Crea un nuevo subbloque a partir de la memoria de este bloque.
		/// @param size Tama�o del subbloque, en bytes.
		/// @param offset Offset del subbloque, en bytes, respecto al inicio de este bloque.
		/// @return Nuevo subbloque.
		/// 
		/// @pre La regi�n de memoria definida por offset y size no debe estar en uso.
		virtual OwnedPtr<IGpuMemorySubblock> CreateNewMemorySubblock(USize64 size, USize64 offset) = 0;


		/// @brief Estructura que se usa cuando un subbloque ya no est� en uso:
		/// una regi�n de memoria entre dos subbloques en uso, pero que
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


		/// @brief Tama�o total del bloque.
		USize64 totalSize = 0;
		/// @brief Espacio que a�n no ha sido usado.
		USize64 availableSpace = 0;
		/// @brief Offset para el pr�ximo subbloque de la zona a�n no usada.
		USize64 currentOffset = 0;

		GpuSharedMemoryType type;
		GpuMemoryUsage usage;
		GpuBufferUsage bufferUsage;

		/// @brief GPU donde reside este bloque.
		IGpu* device = nullptr;

		/// @brief Mutex para la b�squeda y alojamiento de
		/// subbloques.
		mutable MutexHolder m_subblockSearchMutex;

	};

}
