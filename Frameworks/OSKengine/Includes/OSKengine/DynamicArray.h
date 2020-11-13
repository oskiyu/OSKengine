#pragma once

#include <vector>

#include "VectorUtil.h"

#ifdef OSK_EXPERIMENTAL

namespace OSK::Collections {

	//DynamicArray: array que puede cambiar de tamaño.
	template <typename T> class DynamicArray {

	public:


		//"Puntero" al elemento de un DynamicArray.
		class ElementPtr {

			friend class Iterator;
			friend class DynamicArray;

		public:

			//Constructor vacío.
			ElementPtr() {

			}

			//Crea un puntero.
			//	<i>: index del elemento apuntado.
			//	<arr>: DynamicArray al que apunta.
			ElementPtr(const uint32_t& i, DynamicArray& arr) {
				this->i = i;
				collection = &arr;
			}

			//Crea un puntero.
			//	<i>: index del elemento apuntado.
			//	<arr>: DynamicArray al que apunta.
			ElementPtr(const uint32_t& i, DynamicArray* arr) {
				this->i = i;
				collection = arr;
			}

			//Destruye el "puntero".
			~ElementPtr() {

			}

			//Obtiene el valor al que apunta este "puntero".
			inline T& GetValue() const {
				return (*collection)[i];
			}

			//Compara dos punteros.
			//Iguales si señalan al mismo elemento del mismo ArrayList.
			inline bool operator== (const ElementPtr& ptr) const {
				return i == ptr.i && collection == ptr.collection;
			}


			//Compara dos punteros.
			//Iguales si no señalan al mismo elemento del mismo ArrayList.
			inline bool operator!= (const ElementPtr& ptr) const {
				return i != ptr.i || collection != ptr.collection;
			}

		private:

			//Index.
			uint32_t i = 0;

			//Array en el que se almacena el valor apuntado.
			DynamicArray* collection = nullptr;

		};


		//Iterador de un ArrayList.
		//Para el for.
		class Iterator {


		public:

			//Destruye el iterador.
			~Iterator() {

			}

			//Crea el iterador.
			//Internamente usa un ElementPtr.
			Iterator(const uint32_t& i, const DynamicArray& arr) {
				Value.i = i;
				Value.collection = (DynamicArray*)&arr;
			}

			//Señala al elemento siguiente.
			Iterator operator++ () {
				Value.i++;
				return *this;
			}

			//Compara dos iteradores.
			//True si usan internamente el mismo ElementPtr.
			bool operator== (const Iterator& it) const {
				return Value == it.Value;
			}


			//Compara dos iteradores.
			//True si no usan internamente el mismo ElementPtr.
			bool operator!= (const Iterator& it) const {
				return Value != it.Value;
			}

			//Devuelve el valor apuntado por el iterador.
			T& operator* () const {
				return Value.GetValue();
			}

		private:

			//Valor al que se apunta.
			ElementPtr Value;

		};

		/**********************/


		//Crea un DynamicArray vacío.
		//Con espacio para <GrowthFactor> elementos.
		DynamicArray() {
			allocate(GrowthFactor);
		}

		//CopyConstructor.
		DynamicArray(const DynamicArray& original) noexcept {
			size = original.size;
			data = (T*)malloc(sizeof(T) * size);
			memcpy(data, original.data, sizeof(T) * size);
			currentCapacity = original.currentCapacity;
			GrowthFactor = original.GrowthFactor;
		}

		//MoveConstructor.
		DynamicArray(DynamicArray&& original) noexcept {
			size = original.size;
			data = original.data;
			currentCapacity = original.currentCapacity;
			GrowthFactor = original.GrowthFactor;

			original.data = nullptr;
			size = 0;
			currentCapacity = 0;
			GrowthFactor = 0;
		}

		DynamicArray& operator=(DynamicArray&& original) {
			if (&original == this)
				return *this;

			size = original.size;
			data = original.data;
			currentCapacity = original.currentCapacity;
			GrowthFactor = original.GrowthFactor;

			original.data = nullptr;
			size = 0;
			currentCapacity = 0;
			GrowthFactor = 0;

			return *this;
		}

		DynamicArray& operator=(const DynamicArray& original) {
			if (&original == this)
				return *this;

			//Clear();

			size = original.size;
			data = (T*)malloc(sizeof(T) * size);
			memcpy(data, original.data, sizeof(T) * size);
			currentCapacity = original.currentCapacity;
			GrowthFactor = original.GrowthFactor;

			return *this;
		}

		//Elimina el DynamicArray.
		//Libera toda la memoria.
		~DynamicArray() {
			if (AutoDelete)
				_delete();
		}

		//Obtiene el valor del elemento <i>.
		inline T& operator[] (const uint32_t& i) const {
			return data[i];
		}

		//Añade un elemento al DynamicArray.
		inline void AddElement(T element) {

			//Si no cabe, hacemos realloc.
			if (size + 1 > currentCapacity)
				allocate(currentCapacity + GrowthFactor);

			//Actualizamos el elemento del array.
			data[size] = element;

			//Actualizamos el "puntero".
			size++;
		}

		//Añade un elemento al DynamicArray.
		inline void AddElementAsCopy(T element) {

			//Si no cabe, hacemos realloc.
			if (size + 1 > currentCapacity)
				allocate(currentCapacity + GrowthFactor);

			//Actualizamos el elemento del array.
			data[size] = element;

			//Actualizamos el "puntero".
			size++;
		}

		//Obtiene el elemento apuntado por <ptr>.
		inline T& GetElementFromPointer(const ElementPtr& ptr) const {
			return data[ptr.i];
		}

		//Devuelve true si contiene el elemento <element>.
		bool HasElement(const T& element) const {
			for (uint32_t i = 0; i < currentCapacity; i++)
				if (data[i] == element)
					return true;

			return false;
		}

		//Reserva memoria para almacenar <numOfElements> elementos.
		void Reserve(const uint32_t& numOfElements) {
			allocate(numOfElements);
		}

		//Vacía el DynamicArray.
		//No libera la memoria.
		void Empty() {
			size = 0;
		}

		//Vacía el DynamicArray.
		//Vacía la memoria.
		void Clear() {
			_delete();
		}

		//Devuelve el número de elementos en el DynamicArray.
		inline uint32_t GetSize() const {
			return size;
		}

		//Devuelve el número de elementos en el DynamicArray.
		inline uint32_t GetReservedSize() const {
			return currentCapacity;
		}

		//Devuelve el array.
		T* GetData() const {
			return data;
		}

		Iterator begin() const {
			return Iterator(0, *this);
		}

		Iterator end() const {
			return Iterator(size, *this);
		}

		//Por cuanto aumenta el número de espacios reservados cada vez que el DynamicArray tiene que crecer.
		uint32_t GrowthFactor = 5;

		bool AutoDelete = true;

	private:

		void allocate(const uint32_t& size) {
			T* oldData = data;
			data = (T*)malloc(size * sizeof(T));

			if (oldData) {
				memcpy(data, oldData, size * sizeof(T));
				free(oldData);
				oldData = nullptr;
			}

			currentCapacity = size;
		}

		inline void _delete() {
			if (data) {
				free(data);
				data = nullptr;
			}

			currentCapacity = 0;
			size = 0;
		}

		T* data = nullptr;

		uint32_t size = 0;
		uint32_t currentCapacity = 0;

	};

}

#endif