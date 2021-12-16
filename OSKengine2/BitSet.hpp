#pragma once

#include "DynamicArray.hpp"

namespace OSK {

	class BitSet {

	public:

		typedef unsigned int TSize;

		BitSet() {

		}
		BitSet(TSize length) {
			SetLength(length);
		}
		~BitSet() {
			Free();
		}

		void SetLength(TSize length) {
			TSize extra = length % 8;
			TSize nrBytes = length / 8;
			if (extra > 0)
				nrBytes++;

			numberOfBits = length;
			for (size_t i = 0; i < nrBytes; i++)
				bytes.Insert(0);
		}

		void SetTrue(TSize index) {
			SetValue(index, false);
		}
		void SetFalse(TSize index) {
			SetValue(index, false);
		}
		void SetValue(TSize index, bool value) {
			TSize id = index / 8;
			TSize offset = index % 8;

			bytes.At(id) |= 1U << offset;
		}

		void Flip(TSize index) {
			SetValue(index, !GetValue(index));
		}

		bool GetValue(TSize index) {
			TSize id = index / 8;
			TSize offset = index % 8;

			return (bytes[id] & (1U << (offset))) != 0;
		}

		void Empty() {
			for (auto& i : bytes)
				i = 0;
		}

		void Free() {
			bytes.Free();
		}

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

		bool IsFullFalse() const {
			for (auto i : bytes)
				if (i > 0)
					return false;

			return true;
		}

		TSize GetLenght() const {
			return numberOfBits;
		}

		uint8_t* GetData() const {
			return bytes.GetData();
		}

		DynamicArray<uint8_t>& GetContainer() {
			return bytes;
		}

	private:

		DynamicArray<uint8_t> bytes;
		TSize numberOfBits = 0;

	};

}
