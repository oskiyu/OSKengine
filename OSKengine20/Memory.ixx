#include <string>

export module OSKengine.Memory;

export namespace OSK {

	/// <summary>
	/// Devueve la dirección de memoria originalPointer + offset.
	/// </summary>
	/// <param name="originalPointer">Puntero original.</param>
	/// <param name="offset">Offset (en bytes).</param>
	/// <returns>Ptr + offset.</returns>
	inline void* AddPtrOffset(const void* start, size_t offset) {
		const unsigned char* __osk_bytePtr = reinterpret_cast<const unsigned char*>(start);
		__osk_bytePtr += offset;

		return (void*)reinterpret_cast<const void*>(__osk_bytePtr);
	}

}