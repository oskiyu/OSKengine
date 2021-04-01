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

	/*
	Una queue que usa internamente un dynamic array.
	*/
	template <typename T> class ArrayQueue {

	public:

		/*
		Clase que representa un elemento de la cola.
		*/
		class Iterator {

			friend class ArrayQueue;

		public:

			/*
			Obtiene el valor al que apunta.
			*/
			inline T& GetValue(size_t index) const {
				return collection->GetData()[index];
			}

			/*
			Hace que el iterador apunte al siguiente elemento.
			*/
			inline Iterator operator++ () {
				Index++;

				return *this;
			}

			/*
			True si los dos iterators apuntan al mismo elemento del mismo array.
			*/
			inline bool operator== (const Iterator& it) const {
				return Index == it.Index && collection == it.collection;
			}

			/*
			True si los dos iterators no apuntan al mismo elemento del mismo array.
			*/
			inline bool operator!= (const Iterator& it) const {
				return Index != it.Index || collection != it.collection;
			}

			/*
			Get();
			*/
			inline T& operator*() const {
				return GetValue(Index);
			}

		private:

			/*
			Elemento al que apunta.
			*/
			size_t Index = 0;

			/*
			DynArray al que pertenece.
			*/
			ArrayQueue* collection = nullptr;

		};

		/*
		Crea la cola con espacio para <InitialSize> elementos.
		*/
		ArrayQueue() {
			Allocate(InitialSize);
		}

		/*
		Crea una cola con los elementos dados.
		*/
		ArrayQueue(const std::initializer_list<T>& list) {
			Allocate(InitialSize);

			for (auto& i : list)
				Enqueue(i);
		}

		/*
		Constructor de copia.
		*/
		ArrayQueue(const ArrayQueue& arr) {
			CopyContentFrom(arr);
		}

		/*
		Copia el array.
		*/
		ArrayQueue& operator=(const ArrayQueue& arr) {
			if (&arr == this)
				return *this;

			CopyContentFrom(arr);
		}

		/*
		Destruye el array.
		*/
		~ArrayQueue() {
			Delete();
		}

		/*
		Copia el array.
		*/
		void CopyContentFrom(const ArrayQueue& arr) {
			Delete();
			Allocate(arr.Capacity);

			memcpy(Data, arr.Data, sizeof(T) * arr.Capacity);

			GrowthFactor = arr.GrowthFactor;
			GrowthFactorType = arr.GrowthFactorType;
		}

		/*
		Inserta un elemento al final de la cola.
		*/
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

		/*
		Devuelve (y quita) el primer elemento de la cola.
		*/
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

		/*
		Devuelve el iterador que apunta al primer elemento.
		*/
		inline Iterator begin() noexcept {
			return GetIterator(Start);
		}

		/*
		Devuelve el iterador que apunta al último elemento.
		*/
		inline Iterator end() noexcept {
			return GetIterator(End);
		}

		/*
		Devuelve el iterador que apunta a un elemento en particular.
		*/
		inline Iterator GetIterator(size_t index) noexcept {
			Iterator it;
			it.collection = this;
			it.Index = index;

			return it;
		}

		/*
		Elimina y libera memoria.
		*/
		void Free() {
			Delete();
		}

		/*
		Devuelve true si el array está vacío.
		*/
		inline bool IsEmpty() const noexcept {
			return End == 0;
		}

		/*
		Devuelve el número de elementos almacenados.
		*/
		inline size_t GetSize() const noexcept {
			return End - Start;
		}

		/*
		Devuelve el número de elementos reservados.
		*/
		inline size_t GetReservedSize() const noexcept {
			return Capacity;
		}

		/*
		Devuelve el array original.
		*/
		inline T* GetData() const noexcept {
			return Data;
		}

		/*
		Vacía la cola (sin liberar memoria).
		*/
		void Empty() {
			Start = 0;
			End = 0;
		}

		/*
		Devuelve true si el array se ha inicializado.
		*/
		bool HasBeenInitialized(){ 
			return Data != nullptr;
		}

		/*
		Factor de crecimiento.
		*/
		size_t GrowthFactor = 2;

		/*
		Comportamiento al aumentar de tamaño.
		*/
		GrowthFactorType GrowthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		/*
		Reserva espacio para <size> elementos.
		*/
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

		/*
		Elimina el array.
		*/
		void Delete() {
			if (Data) {
				free(Data);
				Data = nullptr;
			}

			Capacity = 0;
			Start = 0;
			End = 0;
		}

		/*
		Array.
		*/
		T* Data = nullptr;

		/*
		Número de espacios reservados.
		*/
		size_t Capacity = 0;

		/*
		Principio de la cola.
		*/
		size_t Start = 0;

		/*
		Fin de la cola.
		*/
		size_t End = 0;

		/*
		Tamaño inicial.
		*/
		const size_t InitialSize = 64;

	};

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE

}