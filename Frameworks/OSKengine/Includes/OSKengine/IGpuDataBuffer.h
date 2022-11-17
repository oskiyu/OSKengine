#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "IGpuMemorySubblock.h"

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;

	/// <summary>
	/// Representa un buffer gen�rico en la GPU.
	/// </summary>
	class OSKAPI_CALL GpuDataBuffer {

	public:

		/// <summary>
		/// Establece los par�metros del buffer.
		/// 
		/// @note No crea el buffer, el buffer es creado por IGpuMemoryAllocator.
		/// </summary>
		/// <param name="buffer">Subbloque que ocupa el buffer.</param>
		/// <param name="size"> Tama�o del buffer. En bytes.</param>
		/// <param name="alignment">Alineamiento del contenido del buffer.</param>
		GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		virtual ~GpuDataBuffer();

		/// <summary>
		/// Mapea toda la memoria.
		/// Todo lo que se escriba en el buffer, tambi�n se escribir� en la GPU.
		/// </summary>
		void MapMemory();

		/// <summary>
		/// Mapea la regi�n de memoria dada.
		/// Todo lo que se escriba en el buffer, tambi�n se escribir� en la GPU.
		/// </summary>
		/// <param name="size"> Tama�o de la regi�n. En bytes. <param>
		/// <param name="offset"> Offset desde el inicio del buffer, en bytes.</param>
		/// 
		/// @pre La regi�n de memoria debe estar contenida dentro del buffer.
		void MapMemory(TSize size, TSize offset);

		/// <summary>
		/// Escribe la informaci�n en el buffer.
		/// </summary>
		/// <param name="data">Informaci�n.</param>
		/// <param name="size">N�mero de bytes a escribir.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre El tama�o a escribir no debe ser mayor que el tama�o de
		/// la regi�n de memoria mapeada.
		/// 
		/// @note Usa el cursor.
		void Write(const void* data, TSize size);

		/// <summary>
		/// Escribe la informaci�n en el buffer.
		/// </summary>
		/// <param name="data">Informaci�n.</param>
		/// <param name="size">N�mero de bytes a escribir.</param>
		/// <param name="offset">Offset (en bytes) respecto al inicio del buffer.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La informaci�n a escribir debe estar contenida dentro
		/// de la zona de memoria mapeada..
		/// 
		/// @note No usa el cursor.
		void WriteOffset(const void* data, TSize size, TSize offset);

		/// <summary>
		/// Desmapea la memoria.
		/// </summary>
		/// 
		/// @pre La memoria debe estar mapeada.
		void Unmap();

		/// <summary>
		/// Escribe la informaci�n del elemento dado en el buffer.
		/// </summary>
		/// <typeparam name="T">Tipo de la estructura a escribir.</typeparam>
		/// <param name="data">Informaci�n.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La informaci�n a escribir debe estar contenida dentro
		/// de la zona de memoria mapeada..
		/// 
		/// @note Usa el cursor.
		template <typename T> void Write(const T& data) {
			Write(&data, sizeof(T));
		}

		/// <summary>
		/// Establece la posici�n del cursor.
		/// </summary>
		/// 
		/// @pre La posici�n debe estar dentro de los l�mites del buffer.
		void SetCursor(TSize position);

		/// <summary>
		/// Establece la posici�n del cursor a 0.
		/// </summary>
		void ResetCursor();

		/// <summary>
		/// Devuelve el tama�o del buffer, en bytes.
		/// </summary>
		TSize GetSize() const;

		/// <summary>
		/// Devuelve la alineaci�nd el buffer, en bytes.
		/// </summary>
		TSize GetAlignment() const;

		IGpuMemorySubblock* GetMemorySubblock() const;
		IGpuMemoryBlock* GetMemoryBlock() const;

	protected:

		UniquePtr<IGpuMemorySubblock> buffer;

	private:

		TSize size = 0;
		TSize alignment = 0;

	};

}
