#pragma once

#include "DynamicArray.hpp"

namespace OSK {

	using TByte = uint8_t;

	/// <summary>
	/// Un bitset representa un array de bits.
	/// 
	/// El número de bits está establecido en tiempo de compilación.
	/// </summary>
	template <TSize size> class ConstexprBitSet {

	public:

		/// <summary>
		/// Crea un bitset.
		/// </summary>
		/// 
		/// @note El valor de los bits será 0.
		ConstexprBitSet() {
			TSize numBytes = size / 8;
			if (size % 8)
				numBytes++;

			Reset();
		}

		/// <summary>
		/// Establece el valor del bit dado a 1.
		/// </summary>
		/// 
		/// @pre Debe ser un índice válido (< size).
		void SetTrue(TSize index) {
			SetValue(index, true);
		}
		/// <summary>
		/// Establece el valor del bit dado a 0.
		/// </summary>
		/// 
		/// @pre Debe ser un índice válido (< size).
		void SetFalse(TSize index) {
			SetValue(index, false);
		}
		/// <summary>
		/// Establece el valor del bit en la posición dada.
		/// </summary>
		/// <param name="index">Índice del bit.</param>
		/// <param name="value">Valor (1 ó 0).</param>
		/// 
		/// @pre Debe ser un índice válido (< size).
		void SetValue(TSize index, bool value) {
			const TSize byteIndex = index / 8;
			const TSize inByteOffset = index % 8;

			value
				? bytes[byteIndex] |=   1U << inByteOffset
				: bytes[byteIndex] &= ~(1U << inByteOffset);
		}

		/// <summary>
		/// Devuelve el valor del bit en la posición dada.
		/// </summary>
		/// 
		/// @pre Debe ser un índice válido (< size).
		bool Get(TSize index) const {
			TSize id = index / 8;
			TSize offset = index % 8;

			return (bytes[id] & (1U << (offset))) != 0;
		}

		/// <summary>
		/// Devuelve el índice del siguiente bit en la secuencia que es 1.
		/// </summary>
		/// <param name="initialIndex">Posición desde la que se comienza la búsqueda.
		/// Incluido en la búsqueda.</param>
		/// 
		/// @note Si no encuentra ningún bit después del dado, devuelve 0.
		TSize GetNextTrueIndex(TSize initialIndex = 0) const {
			TSize index = initialIndex / 8;
			TSize offset = initialIndex % 8;

			for (; index < size; index++) {
				TByte value = bytes[index];

				if (value == 0)
					continue;

				for (TByte i = offset; i < 8; i++)
					if (value & (1 << i))
						return i + index * 8;

				offset = 0;
			}

			return 0;
		}

		/// <summary>
		/// Comprueba si todos los bits valen 0.
		/// </summary>
		bool IsAllFalse() const {
			for (TSize i = 0; i < size; i++)
				if (bytes[i] > 0)
					return false;

			return true;
		}

		/// <summary>
		/// Comprueba si todos los bits valen 1.
		/// </summary>
		bool IsAllTrue() const {
			return !IsAllFalse();
		}

		/// <summary>
		/// Comprueba si el otro bitset tiene en 1 los
		/// bits que este tiene en 1.
		/// </summary>
		bool IsCompatible(const ConstexprBitSet& other) const {
			for (TSize i = 0; i < size; i++)
				if ((bytes[i] & other.bytes[i]) != bytes[i])
					return false;

			return true;
		}

		/// <summary>
		/// Número de bits.
		/// </summary>
		TSize GetLength() const {
			return size;
		}

		/// <summary>
		/// Array con los bits.
		/// 
		/// @note Los bits están almacenados dentro de los bytes.
		/// </summary>
		TByte* GetData() const {
			return bytes;
		}

		/// <summary>
		/// Establece el valor de todos los bits a 0.
		/// </summary>
		void Reset() {
			memset(bytes, 0, size);
		}

	private:

		TByte bytes[size];

	};

}
