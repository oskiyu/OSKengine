#pragma once

#include <corecrt_memory.h>
#include <malloc.h>
#include <string>

#include <initializer_list>
#include "Assert.h"

namespace OSK {

#ifdef OSK_DEBUG
#define OSK_SAFE
#endif

#ifdef OSK_FORCE_SAFE
#ifndef OSK_SAFE
#define OSK_SAFE
#endif
#endif

#ifdef OSK_FORCE_UNSAFE
#ifdef OSK_SAFE
#undef OSK_SAFE
#endif
#endif

	/// <summary>
	/// Factor de crecimiento.
	/// </summary>
	constexpr unsigned int DynamicArrayGrowthFactor = 2;


	/// <summary>
	/// Comportamiento del array cuando no hay espacio:
	/// -EXPONENTIAL: se amplia de manera exponencial(Capacity * Factor).
	///	-LINEAL: se amplia de manera lineal(Capacity + Factor).
	/// </summary>
	enum class GrowthFactorType {

		/// <summary>
		/// Se amplia de manera exponencial(Capacity * Factor).
		/// </summary>
		EXPONENTIAL,

		/// <summary>
		/// Se amplia de manera lineal(Capacity + Factor).
		/// </summary>
		LINEAL

	};

	/// <summary>
	/// Tamaño inicial.
	/// </summary>
	constexpr unsigned int InitialDynamicArraySize = 10;


	/// <summary>
	/// Dynamic array: array que puede cambiar de tamaño.
	/// </summary>
	/// <typeparam name="T">Tipo de elementos que van a ser almacenados.</typeparam>
	template <typename T> class DynamicArray {

	public:

		using TSize = unsigned int;

		/// <summary>
		/// Clase que representa un elemento de un DynamicArray.
		/// Puede no preservarse.
		/// </summary>
		class Iterator {

			friend class DynamicArray;

		public:

			/// <summary>
			/// Crea el iterador.
			/// </summary>
			Iterator(TSize i, const DynamicArray* arr) : index(i), collection(arr) {

			}

			/// <summary>
			/// Hace que el iterador apunte al siguiente elemento.
			/// </summary>
			/// <returns>Self.</returns>
			Iterator operator++ () {
				index++;

				return *this;
			}

			/// <summary>
			/// Hace que el iterador apunte al siguiente elemento.
			/// </summary>
			/// <returns>Self.</returns>
			Iterator operator-- () {
				index--;

				return *this;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator> (const Iterator& other) const {
				return index > other.index;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator< (const Iterator& other) const {
				return index < other.index;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator>= (const Iterator& other) const {
				return index >= other.index;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator<= (const Iterator& other) const {
				return index <= other.index;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator== (const Iterator& it) const {
				return index == it.index;
			}


			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator!= (const Iterator& it) const {
				return index != it.index;
			}

			/// <summary>
			/// Obtiene el valor al que apunta.
			/// </summary>
			T& operator*() const {
				return collection->At(index);
			}

			/// <summary>
			/// True si el iterator es vacío:
			/// no apunta a un elemento válido.
			/// </summary>
			bool IsEmpty() const {
				return isEmpty;
			}

		private:

			/// <summary>
			/// Elemento al que apunta.
			/// </summary>
			TSize index = 0;

			/// <summary>
			/// True si el iterator es vacío.
			/// </summary>
			bool isEmpty = false;

			/// <summary>
			/// Array al que pertenece.
			/// </summary>
			const DynamicArray* collection = nullptr;

		};

#pragma region Constructores y destructores.

		/// <summary>
		/// Crea el dynamic array con espacio para 'initialSize' elementos.
		/// </summary>
		DynamicArray() {
			InitialAllocate(InitialDynamicArraySize);
		}

		/// <summary>
		/// Crea el dynamic array con los elementos dados.
		/// </summary>
		DynamicArray(const std::initializer_list<T>& list) {
			InitialAllocate(list.size());

			for (auto& i : list)
				Insert(i);
		}

		/// <summary>
		/// Constructor de copia.
		/// </summary>
		DynamicArray(const DynamicArray& arr) {
			InitialCopyFrom(arr);
		}

		/// <summary>
		/// Copia el array.
		/// </summary>
		DynamicArray& operator=(const DynamicArray& arr) {
			if (&arr == this)
				return *this;

			CopyFrom(arr);

			return *this;
		}

		/// <summary>
		/// Transfirere el contenido de arr a este array.
		/// </summary>
		DynamicArray(DynamicArray&& arr) {
			InitialCopyFrom(arr);

			arr.Free();
		}

		/// <summary>
		/// Transfirere el contenido de arr a este array.
		/// </summary>
		DynamicArray& operator=(DynamicArray&& arr) {
			if (&arr == this)
				return *this;

			CopyFrom(arr);

			arr.Free();

			return *this;
		}

		/// <summary>
		/// Destruye el array.
		/// </summary>
		~DynamicArray() {
			Free();
		}

		/// <summary>
		/// Copia los contenidos del array.
		/// </summary>
		/// <param name="arr">Array a copiar.</param>
		void CopyFrom(const DynamicArray& arr) {
			Free();
			Allocate(arr.capacity);

			memcpy(data, arr.data, sizeof(T) * arr.capacity);

			growthFactorType = arr.growthFactorType;

			size = arr.size;
		}

		static DynamicArray CreateReservedArray(TSize reservedSize) {
			DynamicArray output;
			output.Reserve(reservedSize);

			return output;
		}

		template <typename... TArgs> static DynamicArray CreateResizedArray(TSize size, TArgs... args) {
			DynamicArray output;
			output.Resize(size, args...);

			return output;
		}

#pragma endregion

#pragma region Funciones

		/// <summary>
		/// Reserva espacio para size elementos.
		/// Preserva los elementos anteriores.
		/// </summary>
		/// <param name="size">Número de elementos.</param>
		void Allocate(TSize size) {
			const TSize oldCapacity = capacity;
			capacity = size;

			data = (T*)realloc(data, sizeof(T) * size);

#ifdef OSK_SAFE
			if (size > 0)
				OSK_ASSERT(data != NULL, "DynamicArray: no se pudo reservar memoria para " + std::to_string(size) + " elementos en el dynamic array.");
#endif
		}

		/// <summary>
		/// Inserta un elemento al final del array.
		/// </summary>
		void Insert(const T& element) {
			if (!HasBeenInitialized())
				Allocate(InitialDynamicArraySize);

			if (size + 1 == capacity) {
				if (growthFactorType == GrowthFactorType::EXPONENTIAL)
					Allocate(capacity * DynamicArrayGrowthFactor);
				else if (growthFactorType == GrowthFactorType::LINEAL)
					Allocate(capacity + DynamicArrayGrowthFactor);
			}

			data[size] = element;

			size++;
		}

		/// <summary>
		/// Inserta un elemento al final del array.
		/// </summary>
		void Insert(T&& element) {
			if (!HasBeenInitialized())
				Allocate(InitialDynamicArraySize);

			if (size + 1 >= capacity) {
				if (growthFactorType == GrowthFactorType::EXPONENTIAL) {
					if (capacity == 0)
						Allocate(InitialDynamicArraySize);
					else
						Allocate(capacity * DynamicArrayGrowthFactor);
				}
				else if (growthFactorType == GrowthFactorType::LINEAL) {
					if (capacity == 0)
						Allocate(InitialDynamicArraySize);
					else
						Allocate(capacity + DynamicArrayGrowthFactor);
				}
			}

			data[size] = std::move(element);

			size++;
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		T& At(TSize index) const {
#ifdef OSK_SAFE
			OSK_ASSERT(index < capacity, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(index < size, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(index >= 0, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to access element from an uninitialized array.");
#endif

			return data[index];
		}

		/// <summary>
		/// Añade el elemento al final de la lista.
		/// </summary>
		void Push(const T& elem) {
			Insert(elem);
		}

		/// <summary>
		/// Añade el elemento al final de la lista.
		/// </summary>
		void Push(T&& elem) {
			Insert(elem);
		}

		/// <summary>
		/// Devuelve el último elemento de la lista. No lo quita de la lista.
		/// </summary>
		T& Peek() {
			return At(size - 1);
		}

		/// <summary>
		/// Devuelve el último elemento de la lista. Lo quita de la lista.
		/// </summary>
		T Pop() {
			T output = At(size - 1);

			RemoveLast();

			return output;
		}

		/// <summary>
		/// Añade los elementos de 'arr' a este array.
		/// </summary>
		/// <param name="arr">Elementos a añadir.</param>
		void InsertAll(const DynamicArray& arr) {
			TSize originalSize = size;

			if (size + arr.GetSize() > capacity)
				Allocate(size + arr.GetSize());

			size += arr.size;

			memcpy(&data[originalSize], arr.data, sizeof(T) * arr.GetSize());
		}

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// </summary>
		inline void Reserve(TSize size) {
			Allocate(size);
		}

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// Se añaden elementos hasta que haya 'size' elementos.
		/// </summary>
		template <typename... TArgs> inline void Resize(TSize size, TArgs... args) {
			TSize oldSize = this->size;
			Reserve(size);

			for (TSize i = oldSize; i < size; i++)
				data[i] = T(args...);

			this->size = size;
		}

		/// <summary>
		/// Elimina el elemento en la posición dada.
		/// Llama al destructor.
		/// </summary>
		void RemoveAt(TSize index) {
#ifdef OSK_SAFE
			OSK_ASSERT(index < capacity, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(index < size, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(index >= 0, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to remove element from an uninitialized array.");
#endif // _DEBUG

			At(index).~T();

			memmove(&data[index], &data[index + 1], size - index);

			size--;
		}

		void Remove(const T& elem) {
			auto it = Find(elem);

			if (!it.IsEmpty())
				RemoveAt(it.index);
		}

		/// <summary>
		/// Elimina el último elemento del array.
		/// </summary>
		void RemoveLast() {
#ifdef OSK_SAFE
			OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to remove element from an uninitialized array.");
#endif // _DEBUG
			At(size - 1).~T();

			size--;
		}

		bool ContainsElement(const T& elem) const {
			return !Find(elem).IsEmpty();
		}

		size_t GetIndexOf(const T& elem) const {
			auto it = Find(elem);

			return it.index;
		}

		Iterator Find(const T& elem) const {
			for (size_t i = 0; i < size; i++)
				if (elem == data[i])
					return Iterator(i, this);

			Iterator output(0, this);
			output.isEmpty = true;

			return output;
		}

		/// <summary>
		/// Elimina el array, liberando memoria.
		/// </summary>
		void Free() {
			if (HasBeenInitialized()) {
				Empty();

				free(data);
				data = nullptr;
			}

			capacity = 0;
			size = 0;
		}

		/// <summary>
		/// Elimina los elementos, sin liberar memoria.
		/// </summary>
		void Empty() {
			for (auto& i : *this)
				i.~T();

			size = 0;
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		/// <param name="i">Posición en el array.</param>
		/// <returns>Elemento.</returns>
		T& operator[] (TSize i) const {
			return At(i);
		}

#pragma endregion

#pragma region Getters & setters

		/// <summary>
		/// Devuelve true si el array se ha inicializado.
		/// </summary>
		/// <returns>Estado del array.</returns>
		bool HasBeenInitialized() const noexcept {
			return data != nullptr;
		}

		/// <summary>
		/// Devuelve true si el array está vacío.
		/// </summary>
		bool IsEmpty() const noexcept {
			return size == 0;
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados.
		/// </summary>
		TSize GetSize() const noexcept {
			return size;
		}

		/// <summary>
		/// Devuelve el número de elementos reservados.
		/// </summary>
		TSize GetReservedSize() const noexcept {
			return capacity;
		}

		/// <summary>
		/// Devuelve el array original.
		/// </summary>
		T* GetData() const noexcept {
			return data;
		}

#pragma endregion

#pragma region Iteradores

		/// <summary>
		/// Devuelve el iterador que apunta al primer elemento.
		/// </summary>
		Iterator begin() const noexcept {
			return GetIterator(0);
		}

		/// <summary>
		/// Devuelve el iterador que apunta al último elemento.
		/// </summary>
		/// <returns>Iterador final.</returns>
		Iterator end() const noexcept {
			return GetIterator(size);
		}

		/// <summary>
		/// Devuelve el iterador que apunta a un elemento en particular.
		/// </summary>
		Iterator GetIterator(TSize index) const noexcept {
			return Iterator(index, this);
		}

#pragma endregion

		/// <summary>
		/// Comportamiento al aumentar de tamaño.
		/// </summary>
		GrowthFactorType growthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		/// <summary>
		/// Reserva espacio para size elementos.
		/// Primer allocate del array..
		/// </summary>
		/// <param name="size">Número de elementos.</param>
		void InitialAllocate(TSize size) {
			capacity = size;
			this->size = 0;

			data = (T*)malloc(sizeof(T) * size);
			memset(data, 0, sizeof(T) * size);

#ifdef OSK_SAFE
			OSK_ASSERT(data != NULL, "DynamicArray: no se pudo reservar memoria para " + std::to_string(size) + " elementos en el dynamic array.");
#endif
		}

		void InitialCopyFrom(const DynamicArray& arr) {
			InitialAllocate(arr.capacity);

			memcpy(data, arr.data, sizeof(T) * arr.capacity);

			growthFactorType = arr.growthFactorType;

			size = arr.size;
		}

		/// <summary>
		/// Array.
		/// </summary>
		T* data = nullptr;

		/// <summary>
		/// Número de espacios reservados.
		/// </summary>
		TSize capacity = 0;

		/// <summary>
		/// Elementos guardados.
		/// </summary>
		TSize size = 0;

	};

	template <typename T> using ArrayStack = DynamicArray<T>;

}

#ifdef OSK_SAFE
#undef OSK_SAFE
#endif // OSK_DS_SAFE_MODE
