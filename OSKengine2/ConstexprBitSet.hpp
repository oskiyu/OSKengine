#pragma once

#include "DynamicArray.hpp"

namespace OSK {

	using TByte = uint8_t;

	template <TSize size> class ConstexprBitSet {

	public:

		ConstexprBitSet() {
			TSize numBytes = size / 8;
			if (size % 8)
				numBytes++;

			bytes.Resize(numBytes);
		}

		void SetTrue(TSize index) {
			SetValue(index, true);
		}
		void SetFalse(TSize index) {
			SetValue(index, false);
		}
		void SetValue(TSize index, bool value) {
			TSize i = index / 8;
			TSize offset = index % 8;

			bytes.At(i) |= 1U << offset;
		}

		bool Get(TSize index) const {
			TSize id = index / 8;
			TSize offset = index % 8;

			return (bytes[id] & (1U << (offset))) != 0;
		}

		bool IsAllFalse() const {
			for (TSize i = 0; i < bytes.GetSize(); i++)
				if (i > 0)
					return false;

			return true;
		}

		bool IsAllTrue() const {
			return !IsAllFalse();
		}

		/// <summary>
		/// Comprueba si el otro bitset tiene en 1 los
		/// bits que este tiene en 1.
		/// </summary>
		bool IsCompatible(const ConstexprBitSet& other) const {
			for (TSize i = 0; i < size; i++)
				if ((bytes.At(i) & other.bytes.At(i)) != bytes.At(i))
					return false;

			return true;
		}

		TSize GetLength() const {
			return size;
		}

		TByte* GetData() const {
			return bytes.GetData();
		}

		void Reset() {
			memset(bytes.GetData(), 0, bytes.GetSize());
		}

	private:

		DynamicArray<TByte> bytes;

	};

}
