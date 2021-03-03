#pragma once

#include <cstdint>
#include <corecrt_memory.h>
#include <malloc.h>
#include <string>
#include <exception>

#include <initializer_list>

namespace OSK {

#ifdef _DEBUG
#define OSK_DS_SAFE_MODE
#endif // _DEBUG


	/*
	Dynamic array: array que puede cambiar de tamaño.
	*/
	template <typename T> class DynamicArray {

	public:

		/*
		Clase que representa un elemento de un DynamicArray.
		Se preserva aunque el array cambie de tamaño.
		*/
		class Iterator {

			friend class DynamicArray;

		public:

			/*
			Obtiene el valor al que apunta.
			*/
			inline T& GetValue(size_t index) const {
				return collection->At(index);
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
			DynamicArray* collection = nullptr;

		};

		/*
		Comportamiento del array cuando no hay espacio:
		-EXPONENTIAL: se amplia de manera exponencial (Capacity * Factor).
		-EXPONENTIAL: se amplia de manera lineal (Capacity + Factor).
		*/
		enum class GrowthFactorType {
			EXPONENTIAL,
			LINEAL
		};

		/*
		Crea el dynamic array con espacio para <InitialSize> elementos.
		*/
		DynamicArray() {
			Allocate(InitialSize);
		}

		/*
		Crea el dynamic array con los elementos dados.
		*/
		DynamicArray(const std::initializer_list<T>& list) {
			Allocate(list.size());

			for (auto& i : list) {
				Insert(i);
			}
		}

		/*
		Constructor de copia.
		*/
		DynamicArray(const DynamicArray& arr) {
			CopyContentFrom(arr);
		}

		/*
		Copia el array.
		*/
		DynamicArray& operator=(const DynamicArray& arr) {
			if (&arr == this) {
				return *this;
			}

			CopyContentFrom(arr);
		}

		/*
		Destruye el array.
		*/
		~DynamicArray() {
			Delete();
		}

		/*
		Copia el array.
		*/
		void CopyContentFrom(const DynamicArray& arr) {
			Delete();
			Allocate(arr.Capacity);

			memcpy(Data, arr.Data, sizeof(T) * arr.Capacity);

			GrowthFactor = arr.GrowthFactor;
			GrowthFactorType = arr.GrowthFactorType;
		}

		/*
		Inserta un elemento al array.
		Puede cambiar de tamaño.
		*/
		void Insert(const T& element) {
			if (!HasBeenInitialized())
				Allocate(InitialSize);

			if (Size + 1 > Capacity) {
				if (GrowthFactorType == GrowthFactorType::EXPONENTIAL)
					Allocate(Capacity * GrowthFactor);
				else if (GrowthFactorType == GrowthFactorType::LINEAL)
					Allocate(Capacity + GrowthFactor);
			}

			Data[Size] = element;

			Size++;
		}

		/*
		Añade los elementos de <arr> a este array.
		*/
		void Concadenate(const DynamicArray& arr) {
			size_t originalSize = Size;

			if (Size + arr.GetSize() > Capacity) {
				Allocate(Size + arr.GetSize());
			}
			else {
				Size += arr.Size;
			}

			memcpy(&Data[originalSize], arr.Data, sizeof(T) * arr.GetSize());
		}

		/*
		Cambia el tamaño del array.
		Los datos se conservan, siempre que se pueda.
		*/
		inline void Resize(size_t size) {
			Allocate(size);
		}

		/*
		Devuelve true si el array está vacío.
		*/
		inline bool IsEmpty() const noexcept {
			return Size == 0;
		}

		/*
		Vacía el array, sin liberar memoria.
		*/
		inline void Empty() const noexcept {
			Size = 0;
		}

		/*
		Devuelve el elemento en la posición dada.
		*/
		inline T& At(size_t index) const {
#ifdef OSK_DS_SAFE_MODE
			if (index >= Capacity) {
				std::string msg = "ERROR: DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(Capacity) + " reserved elements.";

				throw std::runtime_error(msg);
			}

			if (index >= Size) {
				std::string msg = "ERROR: DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(Size) + " elements.";

				throw std::runtime_error(msg);
			}

			if (index < 0) {
				std::string msg = "ERROR: DynamicArray: tried to access element 0.";

				throw std::runtime_error(msg);
			}

			if (!HasBeenInitialized()) {
				std::string msg = "ERROR: DynamicArray: tried to access element from an uninitialized array.";

				throw std::runtime_error(msg);
			}
#endif // _DEBUG

			return Data[index];
		}

		/*
		Elimina el elemento en la posición dada.
		*/
		inline void Remove(size_t index) {
#ifdef OSK_DS_SAFE_MODE
			if (index >= Capacity) {
				std::string msg = "ERROR: DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(Capacity) + " reserved elements.";

				throw std::runtime_error(msg);
			}

			if (index >= Size) {
				std::string msg = "ERROR: DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(Size) + " elements.";

				throw std::runtime_error(msg);
			}

			if (index < 0) {
				std::string msg = "ERROR: DynamicArray: tried to remove element 0.";

				throw std::runtime_error(msg);
			}

			if (!HasBeenInitialized()) {
				std::string msg = "ERROR: DynamicArray: tried to remove element from an uninitialized array.";

				throw std::runtime_error(msg);
			}
#endif // _DEBUG

			T* newData = (T*)malloc(sizeof(T) * (Capacity - index));

			memcpy(newData, &Data[index + 1], sizeof(T) * (Capacity - index));
			memcpy(&Data[index], newData, sizeof(T) * (Capacity - index));

			free(newData);

			Size--;
		}

		inline void RemoveLast() {
#ifdef OSK_DS_SAFE_MODE
			if (!HasBeenInitialized()) {
				std::string msg = "ERROR: DynamicArray: tried to remove element from an uninitialized array.";

				throw std::runtime_error(msg);
			}
#endif // _DEBUG

			Size--;
		}

		/*
		Devuelve el elemento en la posición dada.
		*/
		inline T& operator[] (size_t i) const {
			return At(i);
		}

		/*
		Elimina el array, liberando memoria.
		*/
		inline void Free() {
			Delete();
		}

		/*
		Devuelve el número de elementos almacenados.
		*/
		inline size_t GetSize() const noexcept {
			return Size;
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
		Devuelve el iterador que apunta al primer elemento.
		*/
		inline Iterator begin() noexcept {
			return GetIterator(0);
		}

		/*
		Devuelve el iterador que apunta al último elemento.
		*/
		inline Iterator end() noexcept {
			return GetIterator(Size);
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
		Devuelve true si el array se ha inicializado.
		*/
		inline bool HasBeenInitialized() const noexcept {
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
			Capacity = size;

			T* oldData = Data;
			Data = (T*)malloc(sizeof(T) * size);

			if (oldData) {
				memcpy(Data, oldData, size * sizeof(T));
				free(oldData);
			}
		}

		/*
		Elimina el array.
		*/
		void Delete() {
			if (HasBeenInitialized()) {
				free(Data);
				Data = nullptr;
			}

			Capacity = 0;
			Size = 0;
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
		Elementos guardados.
		*/
		size_t Size = 0;

		/*
		Tamaño inicial.
		*/
		const size_t InitialSize = 10;

	};

}

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
