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
		Cell(const T& value) : value(value) {

		}
		
		/// <summary>
		/// Anterior elemento em la lista.
		/// </summary>
		Cell* previous = nullptr;

		/// <summary>
		/// Siguiente elemento en la lista.
		/// </summary>
		Cell* next = nullptr;

		/// <summary>
		/// Valor almacenado.
		/// </summary>
		T value;

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
			cell = cell->next;
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
			return cell->value;
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
			newCell->next = first;
			first = newCell;
			last = newCell;

			return;
		}

		if (indexPos == currentSize - 1) {
			Last->next = newCell;
			Last = newCell;

			return;
		}

		Cell* current = GetCellAt(indexPos);
		Cell* previous = GetCellAt(indexPos - 1);
		previous->next = newCell;

		newCell->next = current;
	}

	/// <summary>
	/// Devuelve el elemento en la posición dada.
	/// </summary>
	/// <param name="position">Posición del elemento.</param>
	/// <returns>Elemento.</returns>
	T& At(size_t position) {
		return GetCellAt(index)->value;
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
			previous = GetCellAt(index - 1);

		Cell* target = GetCellAt(index);
		Cell* next = target->next;

		if (previous != nullptr)
			previous->next = next;

		delete target;

		if (index == 0 && currentSize > 0)
			first = next;

		if (index == currentSize) {
			last = previous;
		}

		if (currentSize == 0)
			first = nullptr;
	}

	/// <summary>
	/// Devueve true si la lista está vacía.
	/// </summary>
	/// <returns>Estado de la lista.</returns>
	inline bool IsEmpty() const {
		return first == nullptr;
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
		return Iterator(first);
	}

	/// <summary>
	/// Devuelve el iterador que apunta al último elemento.
	/// </summary>
	/// <returns>Iterador final.</returns>
	Iterator end() const {
		if (last == nullptr)
			return Iterator(nullptr);

		return Iterator(last->next);
	}

private:

	/// <summary>
	/// Devuelve la cell que almacena el valor en la posición dada.
	/// </summary>
	/// <param name="index">Posición del elemento.</param>
	/// <returns>Cell que lo contiene.</returns>
	Cell* GetCellAt(const uint32_t& index) const {
		int i = 0;
		Cell* node = first;

		while (i < index) {
			i++;
			node = node->next;
		}

		return node;
	}

	/// <summary>
	/// Primer elemento de la lista.
	/// </summary>
	Cell* first = nullptr;

	/// <summary>
	/// Último elemento de la lista.
	/// </summary>
	Cell* last = nullptr;
	
	/// <summary>
	/// Última célula accedida mediante GetCellAt().
	/// </summary>
	Cell* lastVisited = nullptr;

	/// <summary>
	/// Posición de la última célula accedida mediante GetCellAt().
	/// </summary>
	size_t lastVisitedIndex = 0;

	/// <summary>
	/// Número actual de elementos en la lista.
	/// </summary>
	uint32_t currentSize = 0;

};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
