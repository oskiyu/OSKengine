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
				Index++;

				return *this;
			}

			/// <summary>
			/// Comprueba si dos iteradores apuntan al mismo elemento de la misma colección.
			/// </summary>
			/// <param name="it">Otro iterador.</param>
			/// <returns>True si los dos iterators apuntan al mismo elemento del mismo array.</returns>
			inline bool operator== (const Iterator& it) const {
				return Index == it.Index && collection == it.collection;
			}

			/// <summary>
			/// Comprueba si dos iteradores NO apuntan al mismo elemento de la misma colección.
			/// </summary>
			/// <param name="it">Otro iterador.</param>
			/// <returns>True si los dos iterators NO apuntan al mismo elemento del mismo array.</returns>
			inline bool operator!= (const Iterator& it) const {
				return Index != it.Index || collection != it.collection;
			}

			/// <summary>
			/// Obtiene el valor al que apunta.
			/// </summary>
			/// <returns>Valor al que apunta.</returns>
			inline T& operator*() const {
				return GetValue(Index);
			}

		private:

			/// <summary>
			/// Elemento al que apunta.
			/// </summary>
			size_t Index = 0;

			/// <summary>
			/// ArrayQueue al que pertenece.
			/// </summary>
			ArrayQueue* collection = nullptr;

		};

		/// <summary>
		/// Crea la cola con espacio para <InitialSize> elementos.
		/// </summary>
		ArrayQueue() {
			Allocate(InitialSize);
		}

		/// <summary>
		/// Crea una cola con los elementos dados.
		/// </summary>
		/// <param name="list">Elementos iniciales.</param>
		ArrayQueue(const std::initializer_list<T>& list) {
			Allocate(InitialSize);

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
			Allocate(arr.Capacity);

			memcpy(Data, arr.Data, sizeof(T) * arr.Capacity);

			GrowthFactor = arr.GrowthFactor;
			GrowthFactorType = arr.GrowthFactorType;
		}

		/// <summary>
		/// Inserta un elemento al final de la cola.
		/// </summary>
		/// <param name="element">Elemento a insertar.</param>
		void Enqueue(const T& element) {
			if (!HasBeenInitialized())
				Allocate(InitialSize);

			if (End + 1 > Capacity) {
				if (GrowthFactorType == GrowthFactorType::EXPONENTIAL)
					Allocate(Capacity * GrowthFactor);
				else if (GrowthFactorType == GrowthFactorType::LINEAL)
					Allocate(Capacity + GrowthFactor);
			}

			Data[End] = element;
			End++;
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

			T output = Data[Start];
			Start++;

			if (Start == End)
				Empty();

			return output;
		}

		/// <summary>
		/// Devuelve el iterador que apunta al primer elemento.
		/// </summary>
		/// <returns>Primer iterador.</returns>
		inline Iterator begin() noexcept {
			return GetIterator(Start);
		}

		/// <summary>
		/// Devuelve el iterador que apunta al último elemento.
		/// </summary>
		/// <returns>Último iterador.</returns>
		inline Iterator end() noexcept {
			return GetIterator(End);
		}

		/// <summary>
		/// Devuelve el iterador que apunta a un elemento en particular.
		/// </summary>
		/// <param name="index">Posición del elemento.</param>
		/// <returns>Iterador.</returns>
		inline Iterator GetIterator(size_t index) noexcept {
			Iterator it;
			it.collection = this;
			it.Index = index;

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
			return End == 0;
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados.
		/// </summary>
		/// <returns>Número de elementos.</returns>
		inline size_t GetSize() const noexcept {
			return End - Start;
		}

		/// <summary>
		/// Devuelve el número de elementos reservados.
		/// </summary>
		/// <returns>Elementos reservados.</returns>
		inline size_t GetReservedSize() const noexcept {
			return Capacity;
		}

		/// <summary>
		/// Devuelve el array original.
		/// </summary>
		/// <returns>Puntero al array.</returns>
		inline T* GetData() const noexcept {
			return Data;
		}

		/// <summary>
		/// Vacía la cola (sin liberar memoria).
		/// </summary>
		void Empty() {
			Start = 0;
			End = 0;
		}

		/// <summary>
		/// Devuelve true si el array se ha inicializado.
		/// </summary>
		/// <returns>Estado de la cola.</returns>
		bool HasBeenInitialized(){ 
			return Data != nullptr;
		}

		/// <summary>
		/// Factor de crecimiento. Su comportamiento depende de GrowthFactorType.
		/// </summary>
		size_t GrowthFactor = 2;

		/*
		Comportamiento al aumentar de tamaño.
		*/

		/// <summary>
		/// Comportamiento al aumentar el tamaño del array.
		/// </summary>
		GrowthFactorType GrowthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		/// <summary>
		/// Reserva espacio para <size> elementos.
		/// </summary>
		/// <param name="size">Espacio a reservar.</param>
		void Allocate(size_t size) {
			size_t oldCapacity = Capacity;

			Capacity = size;

			T* oldData = Data;
			Data = (T*)malloc(sizeof(T) * size);

			if (oldData) {
				memcpy(Data, oldData, oldCapacity * sizeof(T));
				free(oldData);
			}
		}

		/// <summary>
		/// Elimina el array.
		/// </summary>
		void Delete() {
			if (Data) {
				free(Data);
				Data = nullptr;
			}

			Capacity = 0;
			Start = 0;
			End = 0;
		}

		/// <summary>
		/// Array.
		/// </summary>
		T* Data = nullptr;

		/// <summary>
		/// Número de espacios reservados.
		/// </summary>
		size_t Capacity = 0;

		/// <summary>
		/// Principio de la cola.
		/// </summary>
		size_t Start = 0;

		/// <summary>
		/// Fin de la cola.
		/// </summary>
		size_t End = 0;

		/// <summary>
		/// Tamaño inicial.
		/// </summary>
		const size_t InitialSize = 64;

	};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE

}