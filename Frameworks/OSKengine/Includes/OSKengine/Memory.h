#pragma once

#include "OSKmacros.h"

/// <summary>
/// Funciones de memoria.
/// </summary>
namespace OSK::Memory {

	/// <summary>
	/// Devueve la dirección de memoria originalPointer + offset.
	/// </summary>
	/// <param name="originalPointer">Puntero original.</param>
	/// <param name="offset">Offset (en bytes).</param>
	/// <returns>Ptr + offset.</returns>
	void* AddPtrOffset(const void* originalPointer, size_t offset);

	/// <summary>
	/// LLama a 'delete', si ptr no es nullptr.
	/// Establece ptr a nullptr.
	/// </summary>
	/// <typeparam name="T">Cualquier tipo.</typeparam>
	/// <param name="ptr">Puntero a eliminar.</param>
	template<typename T> inline void SafeDelete(T** ptr) {
		if (*ptr != nullptr) {
			delete* ptr;
			*ptr = nullptr;
		}
	}

	/// <summary>
	/// LLama a 'delete[]', si ptr no es nullptr.
	/// Establece ptr a nullptr.
	/// </summary>
	/// <typeparam name="T">Cualquier tipo.</typeparam>
	/// <param name="ptr">Array a eliminar.</param>
	template<typename T> inline void SafeDeleteArray(T** ptr) {
		if (*ptr != nullptr) {
			delete[] * ptr;
			*ptr = nullptr;
		}
	}

}
