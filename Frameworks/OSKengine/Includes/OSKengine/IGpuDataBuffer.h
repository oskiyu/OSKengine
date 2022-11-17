#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "IGpuMemorySubblock.h"

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;

	/// <summary>
	/// Representa un buffer genérico en la GPU.
	/// </summary>
	class OSKAPI_CALL GpuDataBuffer {

	public:

		/// <summary>
		/// Establece los parámetros del buffer.
		/// 
		/// @note No crea el buffer, el buffer es creado por IGpuMemoryAllocator.
		/// </summary>
		/// <param name="buffer">Subbloque que ocupa el buffer.</param>
		/// <param name="size"> Tamaño del buffer. En bytes.</param>
		/// <param name="alignment">Alineamiento del contenido del buffer.</param>
		GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		virtual ~GpuDataBuffer();

		/// <summary>
		/// Mapea toda la memoria.
		/// Todo lo que se escriba en el buffer, también se escribirá en la GPU.
		/// </summary>
		void MapMemory();

		/// <summary>
		/// Mapea la región de memoria dada.
		/// Todo lo que se escriba en el buffer, también se escribirá en la GPU.
		/// </summary>
		/// <param name="size"> Tamaño de la región. En bytes. <param>
		/// <param name="offset"> Offset desde el inicio del buffer, en bytes.</param>
		/// 
		/// @pre La región de memoria debe estar contenida dentro del buffer.
		void MapMemory(TSize size, TSize offset);

		/// <summary>
		/// Escribe la información en el buffer.
		/// </summary>
		/// <param name="data">Información.</param>
		/// <param name="size">Número de bytes a escribir.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre El tamaño a escribir no debe ser mayor que el tamaño de
		/// la región de memoria mapeada.
		/// 
		/// @note Usa el cursor.
		void Write(const void* data, TSize size);

		/// <summary>
		/// Escribe la información en el buffer.
		/// </summary>
		/// <param name="data">Información.</param>
		/// <param name="size">Número de bytes a escribir.</param>
		/// <param name="offset">Offset (en bytes) respecto al inicio del buffer.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La información a escribir debe estar contenida dentro
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
		/// Escribe la información del elemento dado en el buffer.
		/// </summary>
		/// <typeparam name="T">Tipo de la estructura a escribir.</typeparam>
		/// <param name="data">Información.</param>
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La información a escribir debe estar contenida dentro
		/// de la zona de memoria mapeada..
		/// 
		/// @note Usa el cursor.
		template <typename T> void Write(const T& data) {
			Write(&data, sizeof(T));
		}

		/// <summary>
		/// Establece la posición del cursor.
		/// </summary>
		/// 
		/// @pre La posición debe estar dentro de los límites del buffer.
		void SetCursor(TSize position);

		/// <summary>
		/// Establece la posición del cursor a 0.
		/// </summary>
		void ResetCursor();

		/// <summary>
		/// Devuelve el tamaño del buffer, en bytes.
		/// </summary>
		TSize GetSize() const;

		/// <summary>
		/// Devuelve la alineaciónd el buffer, en bytes.
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
