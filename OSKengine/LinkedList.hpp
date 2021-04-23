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

	/// <summary>
	/// Almacena el valor T y los punteros.
	/// </summary>
	class Cell {

	public:

		/// <summary>
		/// Crea la cell.
		/// </summary>
		/// <param name="value">Valor almacenado.</param>
		Cell(const T& value) : Value(value) {

		}
		
		/// <summary>
		/// Anterior elemento em la lista.
		/// </summary>
		Cell* Previous = nullptr;

		/// <summary>
		/// Siguiente elemento en la lista.
		/// </summary>
		Cell* Next = nullptr;

		/// <summary>
		/// Valor almacenado.
		/// </summary>
		T Value;

	};

	/// <summary>
	/// Clase que representa un elemento de la lista.
	/// </summary>
	class Iterator {

	public:

		/// <summary>
		/// Crea el iterador, apuntando al elemento de la célula.
		/// </summary>
		/// <param name="cell">Elemento al que apunta.</param>
		Iterator(Cell* cell) {
			this->cell = cell;
		}

		/// <summary>
		/// Hace que el iterador apunte al siguiente elemento.
		/// </summary>
		/// <returns>Self.</returns>
		inline Iterator operator++() {
			cell = cell->Next;
			return *this;
		}

		/// <summary>
		/// Comprueba si dos iteradores apuntan al mismo elemento de la misma colección.
		/// </summary>
		/// <param name="it">Otro iterador.</param>
		/// <returns>True si los dos iterators apuntan al mismo elemento del mismo array.</returns>
		inline bool operator==(const Iterator& it) const {
			return cell == it.cell;
		}

		/// <summary>
		/// Comprueba si dos iteradores NO apuntan al mismo elemento de la misma colección.
		/// </summary>
		/// <param name="it">Otro iterador.</param>
		/// <returns>True si los dos iterators NO apuntan al mismo elemento del mismo array.</returns>
		inline bool operator!=(const Iterator& it) const {
			return cell != it.cell;
		}

		/// <summary>
		/// Obtiene el valor al que apunta.
		/// </summary>
		/// <returns>Valor al que apunta.</returns>
		inline T& operator*() {
			return cell->Value;
		}

	private:

		/// <summary>
		/// Elemento al que apunta.
		/// </summary>
		Cell* cell = nullptr;

	};

	/// <summary>
	/// Inserta un elemento al final de la lista.
	/// </summary>
	/// <param name="value">Elemento insertado.</param>
	inline void Insert(const T& value) {
		Insert(value, GetSize());
	}

	/// <summary>
	/// Inserta un elemento en la posición dada.
	/// El elemento que antes estaba en position, estará en position + 1.
	/// </summary>
	/// <param name="value">Valor insertado.</param>
	/// <param name="position">Posición que va a tomar.</param>
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

	/// <summary>
	/// Devuelve el elemento en la posición dada.
	/// </summary>
	/// <param name="position">Posición del elemento.</param>
	/// <returns>Elemento.</returns>
	T& At(size_t position) {
		return cellAt(index)->Value;
	}

	/// <summary>
	/// Devuelve el elemento en la posición dada.
	/// </summary>
	/// <param name="i">Posición del elemento.</param>
	/// <returns>Elemento.</returns>
	inline T& operator[](size_t i) {
		return At(i);
	}

	/// <summary>
	/// Elimina el elemento en la posición dada. No llama al desctructor.
	/// </summary>
	/// <param name="position">Posición del elemento.</param>
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

	/// <summary>
	/// Devueve true si la lista está vacía.
	/// </summary>
	/// <returns>Estado de la lista.</returns>
	inline bool IsEmpty() const {
		return First == nullptr;
	}

	/// <summary>
	/// Devuelve el número de elementos que hay en la lista.
	/// </summary>
	/// <returns>Elementos de la list.a</returns>
	inline uint32_t GetSize() const {
		return currentSize;
	}

	/// <summary>
	/// Vacía la lista. No llama a los destructores de los elementos.
	/// </summary>
	void MakeNull() {
		while (!IsEmpty()) {
			Remove(0);
		}
	}

	/// <summary>
	/// Devuelve el iterador que apunta al primer elemento.
	/// </summary>
	/// <returns>Primer iterador.</returns>
	Iterator begin() const {
		return Iterator(First);
	}

	/// <summary>
	/// Devuelve el iterador que apunta al último elemento.
	/// </summary>
	/// <returns>Iterador final.</returns>
	Iterator end() const {
		if (Last == nullptr)
			return Iterator(nullptr);

		return Iterator(Last->Next);
	}

private:

	/// <summary>
	/// Devuelve la cell que almacena el valor en la posición dada.
	/// </summary>
	/// <param name="index">Posición del elemento.</param>
	/// <returns>Cell que lo contiene.</returns>
	Cell* cellAt(const uint32_t& index) const {
		int i = 0;
		Cell* node = First;

		while (i < index) {
			i++;
			node = node->Next;
		}

		return node;
	}

	/// <summary>
	/// Primer elemento de la lista.
	/// </summary>
	Cell* First = nullptr;

	/// <summary>
	/// Último elemento de la lista.
	/// </summary>
	Cell* Last = nullptr;
	
	/// <summary>
	/// Última célula accedida mediante cellAt().
	/// </summary>
	Cell* LastVisited = nullptr;

	/// <summary>
	/// Posición de la última célula accedida mediante cellAt().
	/// </summary>
	size_t LastVisitedIndex = 0;

	/// <summary>
	/// Número actual de elementos en la lista.
	/// </summary>
	uint32_t currentSize = 0;

};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
