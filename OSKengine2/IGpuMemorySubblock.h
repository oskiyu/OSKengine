#pragma once

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	class IGpuMemoryBlock;

	/// <summary>
	/// Representa una regi�n de memoria dedicada a un �nico recurso.
	/// 
	/// Puede escribirse informaci�n sobre la GPU a trav�s del subbloque.
	/// Para poder escribir, primero es necesario mapear la memoria necesaria,
	/// y desmapearla al terminar de escribir.
	/// 
	/// Hay dos modos de escritura:
	/// - Write: escribe la informaci�n, y hace avanzar un cursor interno,
	/// de tal manera que la siguiente vez que se llame a esta funci�n, se
	/// escribir� con un offset, para no sobreescribir la informaci�n antes
	/// escrita.
	/// - WriteOffset: ignora el cursor interno.
	/// </summary>
	class OSKAPI_CALL IGpuMemorySubblock {

	public:

		typedef unsigned int TSize;

		virtual ~IGpuMemorySubblock();

		/// <summary>
		/// Mapea toda la memoria del subbloque.
		/// </summary>
		virtual void MapMemory() = 0;
		/// <summary>
		/// Mapea la zona dada.
		/// </summary>
		/// <param name="size">Tama�o de la zona.</param>
		/// <param name="offset">Offset respecto al inicio del subbloque.</param>
		virtual void MapMemory(TSize size, TSize offset) = 0;

		/// <summary>
		/// Escribe informaci�n en la GPU.
		/// </summary>
		virtual void Write(const void* data, TSize size) = 0;
		/// <summary>
		/// Escribe informaci�n en la GPU, con un offset.
		/// </summary>
		virtual void WriteOffset(const void* data, TSize size, TSize offset) = 0;

		/// <summary>
		/// Desmapea el subbloque.
		/// Debe llamarse al terminar de escribir.
		/// </summary>
		virtual void Unmap() = 0;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuMemorySubblock, T> {
			return (T*)this;
		}

		IGpuMemoryBlock* GetOwnerBlock() const;

		TSize GetAllocatedSize() const;
		TSize GetOffsetFromBlock() const;

	protected:

		IGpuMemorySubblock(IGpuMemoryBlock* owner, TSize size, TSize offset);

		IGpuMemoryBlock* ownerBlock = nullptr;

		bool isMapped = false;
		void* mappedData = nullptr;

		TSize reservedSize = 0;
		TSize totalOffsetFromBlock = 0;

		TSize cursor = 0;

	};

}
