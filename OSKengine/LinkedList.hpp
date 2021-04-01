#pragma once


#include <cstdint>
#include <corecrt_memory.h>
#include <malloc.h>
#include <stdexcept>

#ifdef _DEBUG
#define OSK_DS_SAFE_MODE
#endif // _DEBUG

template <typename T> class List {

public:

	/*
	Almacena el valor T y los punteros.
	*/
	class Cell {

	public:

		/*
		Constructor.
		*/
		Cell(const T& value) : Value(value) {

		}
		
		/* 
		Anterior elemento de la lista.
		*/
		Cell* Previous = nullptr;

		/*
		Siguiente elemento de la lista.
		*/
		Cell* Next = nullptr;

		/*
		Valor almacenado.
		*/
		T Value;

	};

	class Iterator {

	public:

		Iterator(Cell* cell) {
			this->cell = cell;
		}

		inline Iterator operator++() {
			cell = cell->Next;
			return *this;
		}

		inline bool operator==(const Iterator& it) const {
			return cell == it.cell;
		}

		inline bool operator!=(const Iterator& it) const {
			return cell != it.cell;
		}

		inline T& operator*() {
			return cell->Value;
		}

	private:

		Cell* cell = nullptr;

	};

	inline void Insert(const T& value) {
		Insert(value, GetSize());
	}

	void Insert(const T& value, size_t position) {
		currentSize++;

		Cell* newCell = new Cell(elem);

		if (indexPos == 0) {
			newCell->Next = First;
			First = newCell;
			Last = newCell;

			return;
		}

		if (indexPos == currentSize - 1) {
			Last->Next = newCell;
			Last = newCell;

			return;
		}

		Cell* current = cellAt(indexPos);
		Cell* previous = cellAt(indexPos - 1);
		previous->Next = newCell;

		newCell->Next = current;
	}

	T& At(size_t position) {
		return cellAt(index)->Value;
	}

	inline T& operator[](size_t i) {
		return At(i);
	}

	void RemoveAt(size_t position) {
		if (IsEmpty())
			return;

		currentSize--;

		Cell* previous = nullptr;
		if (index > 0)
			previous = cellAt(index - 1);

		Cell* target = cellAt(index);
		Cell* next = target->Next;

		if (previous != nullptr)
			previous->Next = next;

		delete target;

		if (index == 0 && currentSize > 0)
			First = next;

		if (index == currentSize) {
			Last = previous;
		}

		if (currentSize == 0)
			First = nullptr;
	}

	inline bool IsEmpty() const {
		return First == nullptr;
	}

	inline uint32_t GetSize() const {
		return currentSize;
	}

	void MakeNull() {
		while (!IsEmpty()) {
			Remove(0);
		}
	}

	Iterator begin() const {
		return Iterator(First);
	}

	Iterator end() const {
		if (Last == nullptr)
			return Iterator(nullptr);

		return Iterator(Last->Next);
	}

private:

	Cell* cellAt(const uint32_t& index) const {
		int i = 0;
		Cell* node = First;

		while (i < index) {
			i++;
			node = node->Next;
		}

		return node;
	}

	Cell* First = nullptr;
	Cell* Last = nullptr;
	
	Cell* LastVisited = nullptr;
	size_t LastVisitedIndex = 0;

	uint32_t currentSize = 0;

};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
