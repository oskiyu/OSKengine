#pragma once

#include "DynamicArray.hpp"

namespace OSK {

	/// <summary>
	/// Un bitset representa un array de bits.
	/// </summary>
	class BitSet {

	public:

		/// <summary>
		/// Crea un bitset vacío.
		/// </summary>
		/// 
		/// @warning Deja el bitset en un estado inválido:
		/// se debe establecer la longitud del bitset para poder ser usado.
		BitSet() {

		}

		/// <summary>
		/// Crea un bitset con el número de bits dado.
		/// </summary>
		/// 
		/// @note El valor de los bits será 0.
		BitSet(TSize length) {
			SetLength(length);
		}

		~BitSet() {
			Free();
		}

		/// <summary>
		/// Establece el número de bist del bitset.
		/// </summary>
		/// 
		/// @note Si el número de bits es mayor al actual, los nuevos bits
		/// tendrán un valor de 0.
		void SetLength(TSize length) {
			TSize extra = length % 8;
			TSize nrBytes = length / 8;
			if (extra > 0)
				nrBytes++;

			numberOfBits = length;
			for (size_t i = 0; i < nrBytes; i++)
				bytes.Insert(0);
		}

		/// <summary>
		/// Establece el valor del bit dado a 1.
		/// </summary>
		/// 
		/// @pre Debe ser un índice válido (< size).
		void SetTrue(TSize index) {
			SetValue(index, false);
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
			TSize id = index / 8;
			TSize offset = index % 8;

			bytes.At(id) |= 1U << offset;
		}

		/// <summary>
		/// Cambia el valor del bit en la posición dada.
		/// </summary>
		/// 
		/// @note 0 -> 1.
		/// @note 1 -> 0.
		/// 
		/// @pre Debe ser un índice válido (< size).
		void Flip(TSize index) {
			SetValue(index, !GetValue(index));
		}

		/// <summary>
		/// Devuelve el valor del bit en la posición dada.
		/// </summary>
		/// 
		/// @pre Debe ser un índice válido (< size).
		bool GetValue(TSize index) {
			TSize id = index / 8;
			TSize offset = index % 8;

			return (bytes[id] & (1U << (offset))) != 0;
		}

		/// <summary>
		/// Establece el valor de todos los bits a 0.
		/// </summary>
		void Empty() {
			for (auto& i : bytes)
				i = 0;
		}

		void Free() {
			bytes.Free();
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

			for (; index < bytes.GetSize(); index++) {
				uint8_t value = bytes.At(index);

				if (value == 0)
					continue;

				for (uint8_t i = offset; i < 8; i++)
					if (value & (1 << i))
						return i + index * 8;

				offset = 0;
			}

			return 0;
		}

		/// <summary>
		/// Comprueba si todos los bits valen 0.
		/// </summary>
		bool IsFullFalse() const {
			for (auto i : bytes)
				if (i > 0)
					return false;

			return true;
		}

		/// <summary>
		/// Número de bits.
		/// </summary>
		TSize GetLenght() const {
			return numberOfBits;
		}

		/// <summary>
		/// Array con los bits.
		/// 
		/// @note Los bits están almacenados dentro de los bytes.
		/// </summary>
		uint8_t* GetData() const {
			return bytes.GetData();
		}

		/// <summary>
		/// DynamicArray con los bits.
		/// 
		/// @note Los bits están almacenados dentro de los bytes.
		/// </summary>
		DynamicArray<uint8_t>& GetContainer() {
			return bytes;
		}

	private:

		DynamicArray<uint8_t> bytes;
		TSize numberOfBits = 0;

	};

}
