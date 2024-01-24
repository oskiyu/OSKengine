#pragma once

#include "OSKmacros.h"
#include <type_traits>

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;

	/// @brief Representa una región de memoria dedicada a un único recurso.
	/// 
	/// Puede escribirse información sobre la GPU a través del subbloque.
	/// Para poder escribir, primero es necesario mapear la memoria necesaria,
	/// y desmapearla al terminar de escribir.
	/// 
	/// Hay dos modos de escritura:
	/// - Write: escribe la información, y hace avanzar un cursor interno,
	/// de tal manera que la siguiente vez que se llame a esta función, se
	/// escribirá con un offset, para no sobreescribir la información antes
	/// escrita.
	/// - WriteOffset: ignora el cursor interno.
	class OSKAPI_CALL IGpuMemorySubblock {

	public:

		virtual ~IGpuMemorySubblock();

		OSK_DEFINE_AS(IGpuMemorySubblock);
		
		
		/// @brief Mapea toda la memoria del subbloque, para poder escribirse.
		/// @pre El tipo de memoria debe ser GPU_AND_CPU.
		/// @pre No debe haber ninguna región de memoria mapeada.
		/// @post Se podrá escribir en la zona mapeada.
		virtual void MapMemory() = 0;
		
		/// @brief Mapea la zona dada.
		/// @param size Tamaño de la zona, en bytes.
		/// @param offset Offset respecto al inicio del subbloque, en bytes.
		/// 
		/// @pre El tipo de memoria debe ser GPU_AND_CPU.
		/// @pre La región indicada debe estar dentro de los límites de este subbloque.
		/// @pre No debe haber ninguna región de memoria mapeada.
		/// 
		/// @post Se podrá escribir en la zona mapeada.
		virtual void MapMemory(USize64 size, USize64 offset) = 0;


		/// @brief Escribe información en la región de memoria.
		/// Usa el cursor interno, por lo que sucesivas llamdas a esta función
		/// no se sobreescribirán, sino que escribirán una después de otra.
		/// 
		/// @param data Datos a escribir.
		/// @param size Tamaño de los datos, en bytes.
		/// 
		/// @pre La memoria debe haber sido mapeada antes.
		/// @pre Toda la información escrita debe escribirse sobre una zona de memoria
		/// completamente mapeada.
		/// 
		/// @throws GpuMemoryNotMappedException Si la memoria no está mapeada.
		virtual void Write(const void* data, USize64 size) = 0;
		
		/// @brief Escribe información en la región de memoria.
		/// No usa el cursor interno
		/// @param data Datos a escribir.
		/// @param size Tamaño de los datos, en bytes.
		/// @param offset Offset a partir del que se comienza a escribir.
		/// Respecto al inicio del subbloque.
		/// 
		/// @pre La memoria debe haber sido mapeada antes.
		/// @pre Toda la información escrita debe escribirse sobre una zona de memoria
		/// completamente mapeada.
		/// 
		/// @throws GpuMemoryNotMappedException Si la memoria no está mapeada.
		virtual void WriteOffset(const void* data, USize64 size, USize64 offset) = 0;


		/// @brief Desmapea el subbloque.
		/// Debe llamarse al terminar de escribir.
		/// 
		/// @pre Debe haber alguna región de memoria mapeada.
		/// @post Ya no se podrá escribir en la memoria al no ser que se vuelva a mapear.
		virtual void Unmap() = 0;


		/// @brief Establece el cursor interno en la posición dada.
		/// @param position Offset, en bytes, respecto al inicio del subbloque.
		void SetCursor(USize64 position);

		/// @brief Establece el cursor a su posición inicial (al principio del subbloque).
		void ResetCursor();

 
		IGpuMemoryBlock* GetOwnerBlock() const;

		USize64 GetAllocatedSize() const;
		USize64 GetOffsetFromBlock() const;

	protected:

		IGpuMemorySubblock(IGpuMemoryBlock* owner, USize64 size, USize64 offset);

		IGpuMemoryBlock* ownerBlock = nullptr;

		bool isMapped = false;
		void* mappedData = nullptr;

		USize64 reservedSize = 0;
		USize64 totalOffsetFromBlock = 0;

		USize64 cursor = 0;

	};

}
