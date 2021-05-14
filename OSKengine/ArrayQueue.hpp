#pragma once

#include <initializer_list>

#include <cstdint>
#include <corecrt_memory.h>
#include <malloc.h>
#include <exception>

#include "GrowthFactorType.h"

namespace OSK {

#ifdef _DEBUG
#define OSK_DS_SAFE_MODE
#endif

	/// <summary>
	/// Una queue que usa internamente un dynamic array.
	/// </summary>
	/// <typeparam name="T">Tipo de objetos en el ArrayQueue.</typeparam>
	template <typename T> class ArrayQueue {

	public:

		/// <summary>
		/// Clase que representa un elemento de la cola.
		/// </summary>
		class Iterator {

			friend class ArrayQueue;

		public:
			
			/// <summary>
			/// Obtiene el valor en la posición dada.
			/// </summary>
			/// <param name="index">Posición en el array.</param>
			/// <returns>Valor en esa posición.</returns>
			inline T& GetValue(size_t index) const {
				return collection->GetData()[index];
			}

			/// <summary>
			/// Hace que el iterador apunte al siguiente elemento.
			/// </summary>
			/// <returns>Self.</returns>
			inline Iterator operator++ () {
				index++;

				return *this;
			}

			/// <summary>
			/// Comprueba si dos iteradores apuntan al mismo elemento de la misma colección.
			/// </summary>
			/// <param name="it">Otro iterador.</param>
			/// <returns>True si los dos iterators apuntan al mismo elemento del mismo array.</returns>
			inline bool operator== (const Iterator& it) const {
				return index == it.index && collection == it.collection;
			}

			/// <summary>
			/// Comprueba si dos iteradores NO apuntan al mismo elemento de la misma colección.
			/// </summary>
			/// <param name="it">Otro iterador.</param>
			/// <returns>True si los dos iterators NO apuntan al mismo elemento del mismo array.</returns>
			inline bool operator!= (const Iterator& it) const {
				return index != it.index || collection != it.collection;
			}

			/// <summary>
			/// Obtiene el valor al que apunta.
			/// </summary>
			/// <returns>Valor al que apunta.</returns>
			inline T& operator*() const {
				return GetValue(index);
			}

		private:

			/// <summary>
			/// Elemento al que apunta.
			/// </summary>
			size_t index = 0;

			/// <summary>
			/// ArrayQueue al que pertenece.
			/// </summary>
			ArrayQueue* collection = nullptr;

		};

		/// <summary>
		/// Crea la cola con espacio para <InitialSize> elementos.
		/// </summary>
		ArrayQueue() {
			Allocate(initialSize);
		}

		/// <summary>
		/// Crea una cola con los elementos dados.
		/// </summary>
		/// <param name="list">Elementos iniciales.</param>
		ArrayQueue(const std::initializer_list<T>& list) {
			Allocate(initialSize);

			for (auto& i : list)
				Enqueue(i);
		}

		/// <summary>
		/// Constructor de copia.
		/// </summary>
		/// <param name="arr">Array que se va a copiar.</param>
		ArrayQueue(const ArrayQueue& arr) {
			CopyContentFrom(arr);
		}

		/// <summary>
		/// Copia el array.
		/// </summary>
		/// <param name="arr">Array a copiar.</param>
		/// <returns>Self.</returns>
		ArrayQueue& operator=(const ArrayQueue& arr) {
			if (&arr == this)
				return *this;

			CopyContentFrom(arr);
		}

		/// <summary>
		/// Destruye el array.
		/// </summary>
		~ArrayQueue() {
			Delete();
		}

		/// <summary>
		/// Copia los contenidos del array.
		/// </summary>
		/// <param name="arr">Array a copiar.</param>
		void CopyContentFrom(const ArrayQueue& arr) {
			Delete();
			Allocate(arr.capacity);

			memcpy(data, arr.data, sizeof(T) * arr.capacity);

			growthFactor = arr.growthFactor;
			growthFactorType = arr.growthFactorType;
		}

		/// <summary>
		/// Inserta un elemento al final de la cola.
		/// </summary>
		/// <param name="element">Elemento a insertar.</param>
		void Enqueue(const T& element) {
			if (!HasBeenInitialized())
				Allocate(initialSize);

			if (endIndex + 1 > capacity) {
				if (growthFactorType == GrowthFactorType::EXPONENTIAL)
					Allocate(capacity * growthFactor);
				else if (growthFactorType == GrowthFactorType::LINEAL)
					Allocate(capacity + growthFactor);
			}

			data[endIndex] = element;
			endIndex++;
		}

		/// <summary>
		/// Devuelve (y quita) el primer elemento de la cola.
		/// </summary>
		/// <returns>El primer elemento de la cola.</returns>
		T Dequeue() {
#ifdef OSK_DS_SAFE_MODE
			if (IsEmpty()) {
				std::string msg = "ERROR: ArrayQueue: la cola está vacía.";

				throw std::runtime_error(msg);
			}

			if (!HasBeenInitialized()) {
				std::string msg = "ERROR: ArrayQueue: la no ha sido inicializada.";

				throw std::runtime_error(msg);
			}
#endif // OSK_DS_SAFE_MODE

			T output = data[startIndex];
			startIndex++;

			if (startIndex == endIndex)
				Empty();

			return output;
		}

		/// <summary>
		/// Devuelve el iterador que apunta al primer elemento.
		/// </summary>
		/// <returns>Primer iterador.</returns>
		inline Iterator begin() noexcept {
			return GetIterator(startIndex);
		}

		/// <summary>
		/// Devuelve el iterador que apunta al último elemento.
		/// </summary>
		/// <returns>Último iterador.</returns>
		inline Iterator end() noexcept {
			return GetIterator(endIndex);
		}

		/// <summary>
		/// Devuelve el iterador que apunta a un elemento en particular.
		/// </summary>
		/// <param name="index">Posición del elemento.</param>
		/// <returns>Iterador.</returns>
		inline Iterator GetIterator(size_t index) noexcept {
			Iterator it;
			it.collection = this;
			it.index = index;

			return it;
		}

		/// <summary>
		/// Libera memoria.
		/// </summary>
		void Free() {
			Delete();
		}

		/// <summary>
		/// Devuelve true si el array está vacío.
		/// </summary>
		/// <returns>Estado de la cola.</returns>
		inline bool IsEmpty() const noexcept {
			return endIndex == 0;
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados.
		/// </summary>
		/// <returns>Número de elementos.</returns>
		inline size_t GetSize() const noexcept {
			return endIndex - startIndex;
		}

		/// <summary>
		/// Devuelve el número de elementos reservados.
		/// </summary>
		/// <returns>Elementos reservados.</returns>
		inline size_t GetReservedSize() const noexcept {
			return capacity;
		}

		/// <summary>
		/// Devuelve el array original.
		/// </summary>
		/// <returns>Puntero al array.</returns>
		inline T* GetData() const noexcept {
			return data;
		}

		/// <summary>
		/// Vacía la cola (sin liberar memoria).
		/// </summary>
		void Empty() {
			startIndex = 0;
			endIndex = 0;
		}

		/// <summary>
		/// Devuelve true si el array se ha inicializado.
		/// </summary>
		/// <returns>Estado de la cola.</returns>
		bool HasBeenInitialized(){ 
			return data != nullptr;
		}

		/// <summary>
		/// Factor de crecimiento. Su comportamiento depende de GrowthFactorType.
		/// </summary>
		size_t growthFactor = 2;

		/*
		Comportamiento al aumentar de tamaño.
		*/

		/// <summary>
		/// Comportamiento al aumentar el tamaño del array.
		/// </summary>
		GrowthFactorType growthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		/// <summary>
		/// Reserva espacio para <size> elementos.
		/// </summary>
		/// <param name="size">Espacio a reservar.</param>
		void Allocate(size_t size) {
			size_t oldCapacity = capacity;

			capacity = size;

			T* oldData = data;
			data = (T*)malloc(sizeof(T) * size);

			if (oldData) {
				memcpy(data, oldData, oldCapacity * sizeof(T));
				free(oldData);
			}
		}

		/// <summary>
		/// Elimina el array.
		/// </summary>
		void Delete() {
			if (data) {
				free(data);
				data = nullptr;
			}

			capacity = 0;
			startIndex = 0;
			endIndex = 0;
		}

		/// <summary>
		/// Array.
		/// </summary>
		T* data = nullptr;

		/// <summary>
		/// Número de espacios reservados.
		/// </summary>
		size_t capacity = 0;

		/// <summary>
		/// Principio de la cola.
		/// </summary>
		size_t startIndex = 0;

		/// <summary>
		/// Fin de la cola.
		/// </summary>
		size_t endIndex = 0;

		/// <summary>
		/// Tamaño inicial.
		/// </summary>
		const size_t initialSize = 64;

	};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE

}