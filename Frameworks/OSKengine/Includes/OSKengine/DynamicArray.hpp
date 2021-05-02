#pragma once

#include <cstdint>
#include <corecrt_memory.h>
#include <malloc.h>
#include <string>
#include <exception>

#include <initializer_list>

#include "GrowthFactorType.h"

namespace OSK {

#ifdef _DEBUG
#define OSK_DS_SAFE_MODE
#endif // _DEBUG

	/// <summary>
	/// Dynamic array: array que puede cambiar de tamaño.
	/// </summary>
	/// <typeparam name="T">Tipo de elementos que van a ser almacenados.</typeparam>
	template <typename T> class DynamicArray {

	public:

		/// <summary>
		/// Clase que representa un elemento de un DynamicArray.
		/// Se preserva aunque el array cambie de tamaño.
		/// </summary>
		class Iterator {

			friend class DynamicArray;

		public:

			/// <summary>
			/// Obtiene el valor en la posición dada.
			/// </summary>
			/// <param name="index">Posición en el array.</param>
			/// <returns>Valor en esa posición.</returns>
			inline T& GetValue(size_t index) const {
				return collection->At(index);
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
			/// Array al que pertenece.
			/// </summary>
			DynamicArray* collection = nullptr;

		};

		/// <summary>
		/// Crea el dynamic array con espacio para <InitialSize> elementos.
		/// </summary>
		DynamicArray() {
			Allocate(InitialSize);
		}

		/// <summary>
		/// Crea el dynamic array con los elementos dados.
		/// </summary>
		/// <param name="list">Elementos iniciales.</param>
		DynamicArray(const std::initializer_list<T>& list) {
			Allocate(list.size());

			for (auto& i : list) {
				Insert(i);
			}
		}

		/// <summary>
		/// Constructor de copia.
		/// </summary>
		/// <param name="arr">Array que se va a copiar.</param>
		DynamicArray(const DynamicArray& arr) {
			CopyContentFrom(arr);
		}

		/// <summary>
		/// Copia el array.
		/// </summary>
		/// <param name="arr">Array a copiar.</param>
		/// <returns>Self.</returns>
		DynamicArray& operator=(const DynamicArray& arr) {
			if (&arr == this) {
				return *this;
			}

			CopyContentFrom(arr);
		}

		/// <summary>
		/// Destruye el array.
		/// </summary>
		~DynamicArray() {
			Delete();
		}

		/// <summary>
		/// Copia los contenidos del array.
		/// </summary>
		/// <param name="arr">Array a copiar.</param>
		void CopyContentFrom(const DynamicArray& arr) {
			Delete();
			Allocate(arr.Capacity);

			memcpy(Data, arr.Data, sizeof(T) * arr.Capacity);

			GrowthFactor = arr.GrowthFactor;
			GrowthFactorType = arr.GrowthFactorType;
		}

		/// <summary>
		/// Inserta un elemento al array.
		/// Puede cambiar de tamaño.
		/// </summary>
		/// <param name="element">Elemento a añadir.</param>
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

		/// <summary>
		/// Añade los elementos de arr a este array.
		/// </summary>
		/// <param name="arr">Elementos a añadir.</param>
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

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// </summary>
		/// <param name="size">Nuevo tamaño del array.</param>
		inline void Resize(size_t size) {
			Allocate(size);
		}

		/// <summary>
		/// Devuelve true si el array está vacío.
		/// </summary>
		/// <returns>Estado del array.</returns>
		inline bool IsEmpty() const noexcept {
			return Size == 0;
		}

		/// <summary>
		/// Vacía el array, sin liberar memoria.
		/// </summary>
		inline void Empty() const noexcept {
			Size = 0;
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		/// <param name="index">Posición del elemento.</param>
		/// <returns>Elemento.</returns>
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

		/// <summary>
		/// Elimina el elemento en la posición dada.
		/// </summary>
		/// <param name="index">Posición del elemento.</param>
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

		/// <summary>
		/// Elimina el elemento en la posición dada. El último elemento del array pasa a su lugar.
		/// </summary>
		/// <param name="index">Posición del elemento.</param>
		inline void RemoveAndMoveLast(size_t index) {
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

			At(index) = At(Size - 1);

			Size--;
		}

		/// <summary>
		/// Elimina el último elemento del array.
		/// </summary>
		inline void RemoveLast() {
#ifdef OSK_DS_SAFE_MODE
			if (!HasBeenInitialized()) {
				std::string msg = "ERROR: DynamicArray: tried to remove element from an uninitialized array.";

				throw std::runtime_error(msg);
			}
#endif // _DEBUG

			Size--;
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		/// <param name="i">Posición en el array.</param>
		/// <returns>Elemento.</returns>
		inline T& operator[] (size_t i) const {
			return At(i);
		}

		/// <summary>
		/// Elimina el array, liberando memoria.
		/// </summary>
		inline void Free() {
			Delete();
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados.
		/// </summary>
		/// <returns>Elementos en el array.</returns>
		inline size_t GetSize() const noexcept {
			return Size;
		}

		/// <summary>
		/// Devuelve el número de elementos reservados.
		/// </summary>
		/// <returns>Espacio reservado.</returns>
		inline size_t GetReservedSize() const noexcept {
			return Capacity;
		}

		/// <summary>
		/// Devuelve el array original.
		/// </summary>
		/// <returns>Array.</returns>
		inline T* GetData() const noexcept {
			return Data;
		}

		/// <summary>
		/// Devuelve el iterador que apunta al primer elemento.
		/// </summary>
		/// <returns>Primer iterador.</returns>
		inline Iterator begin() noexcept {
			return GetIterator(0);
		}

		/// <summary>
		/// Devuelve el iterador que apunta al último elemento.
		/// </summary>
		/// <returns>Iterador final.</returns>
		inline Iterator end() noexcept {
			return GetIterator(Size);
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
		/// Devuelve la posición del elemento dado.
		/// Devuelve Size + 1 si no está en el array.
		/// </summary>
		/// <param name="value">Valor buscado.</param>
		/// <returns>Su posición.</returns>
		inline size_t GetPosition(const T& value) const noexcept {
			for (size_t i = 0; i < Size; i++) {
				if (At(i) == value) {
					return i;
				}
			}

			return Size + 1;
		}

		/// <summary>
		/// Devuelve true si el array tiene el elemento dado.
		/// </summary>
		/// <param name="value">Valor buscado.</param>
		/// <returns>True si está en el array.</returns>
		bool HasElement(const T& value) const noexcept {
			for (size_t i = 0; i < Size; i++)
				if (At(i) == value)
					return true;

			return false;
		}

		/// <summary>
		/// Devuelve true si el array se ha inicializado.
		/// </summary>
		/// <returns>Estado del array.</returns>
		inline bool HasBeenInitialized() const noexcept {
			return Data != nullptr;
		}

		/// <summary>
		/// Factor de crecimiento.
		/// </summary>
		size_t GrowthFactor = 2;
		/// <summary>
		/// Comportamiento al aumentar de tamaño.
		/// </summary>
		GrowthFactorType GrowthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		/// <summary>
		/// Reserva espacio para size elementos.
		/// </summary>
		/// <param name="size">Número de elementos.</param>
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
			if (HasBeenInitialized()) {
				free(Data);
				Data = nullptr;
			}

			Capacity = 0;
			Size = 0;
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
		/// Elementos guardados.
		/// </summary>
		size_t Size = 0;

		/// <summary>
		/// Tamaño inicial.
		/// </summary>
		const size_t InitialSize = 10;

	};

}

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
