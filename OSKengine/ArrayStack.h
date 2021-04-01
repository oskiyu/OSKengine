#pragma once

#include <cstdint>
#include <corecrt_memory.h>
#include <malloc.h>
#include <stdexcept>

#include "DynamicArray.hpp"

#ifdef _DEBUG
#define OSK_DS_SAFE_MODE
#endif // _DEBUG

namespace OSK {

	/*
	Un stack que usa un array por dentro.
	*/
	template <typename T> class ArrayStack {

	public:

		/*
		Crea un stack sin inicializar.
		*/
		ArrayStack() {

		}

		/*
		Crea un stack con capacidad para <numberOfElements>.
		*/
		ArrayStack(size_t numberOfElements) {
			Allocate(numberOfElements);
		}

		/*
		Crea un stack con los elementos de <stack>.
		*/
		ArrayStack(const ArrayStack& stack) {
			CopyFrom(stack);
		}

		/*
		Copia el stack.
		*/
		ArrayStack& operator=(const ArrayStack& stack) {
			if (&stack == this) {
				return this;
			}

			CopyFrom(stack);
		}

		/*
		Destruye el stack.
		*/
		~ArrayStack() {
			Free();
		}

		/*
		Reserva memoria para <maxElements> elementos.
		*/
		void Allocate(size_t maxElements) {
			if (HasBeenInitialized()) {
				Free();
			}

			Data = (T*)malloc(maxElements * sizeof(T));
			MaxSize = maxElements;
		}

		/*
		Introduce un elemento al stack.
		*/
		void Push(const T& element) {
#ifdef OSK_DS_SAFE_MODE
			if (IsFull()) {
				throw std::runtime_error("ERROR: stack is full.");
			}
#endif
			Data[CurrentSize] = element;
			CurrentSize++;
		}

		/*
		Devuelve el último elemento que se haya añadido.
		*/
		T Pop() {
#ifdef OSK_DS_SAFE_MODE
			if (IsEmpty()) {
				throw std::runtime_error("ERROR: stack is empty.");
			}
#endif

			T element = Data[CurrentSize - 1];
			CurrentSize--;

			return element;
		}

		/*
		Elimina los elementos, sin liberar memoria.
		*/
		void Clear() {
			CurrentSize = 0;
		}

		/*
		Copia los elementos del stack.
		*/
		void CopyFrom(const ArrayStack& stack) {
			Free();

			Allocate(stack.MaxSize);
			memcpy(Data, stack.Data, sizeof(T) * MaxSize);
		}

		/*
		Libera memoria.
		*/
		void Free() {
			if (HasBeenInitialized()) {
				free(Data);
				Data = nullptr;
			}

			CurrentSize = 0;
		}

		/*
		Devuelve el número de elementos almacenados en el stack.
		*/
		inline size_t GetSize() const {
			return CurrentSize;
		}

		/*
		Devuelve el número máximo de elementos que se pueden almacenar.
		*/
		inline size_t GetMaxSize() const {
			return MaxSize;
		}

		/*
		Devuelve true si el stack está vacío.
		*/
		inline bool IsEmpty() const {
			return GetSize() == 0;
		}

		/*
		Devuelve true si el stack está lleno.
		*/
		inline bool IsFull() const {
			return GetSize() == GetMaxSize();
		}

		/*
		Devuelve true si se ha resrvado memoria.
		*/
		inline bool HasBeenInitialized() const {
			return Data != nullptr;
		}

	private:

		/*
		Array.
		*/
		T* Data = nullptr;
				
		/*
		Máximo número de elementos.
		*/
		size_t MaxSize = 0;

		/*
		Número de elementos guardados actualmente.
		*/
		size_t CurrentSize = 0;

	};

	template <typename T> class Stack {

	public:

		void Push(const T& element) {
			Array.Insert(element);
		}

		T Pop() {
			T value = Array.At(Array.GetSize() - 1);
			Array.RemoveLast();
			return value;
		}

		/*
		Copia los elementos del stack.
		*/
		void CopyFrom(const Stack& stack) {
			Array.CopyContentFrom(stack.Array);
		}

		/*
		Libera memoria.
		*/
		void Free() {
			Array.Free();
		}

		/*
		Devuelve el número de elementos almacenados en el stack.
		*/
		inline size_t GetSize() const {
			return Array.GetSize();
		}

		/*
		Devuelve true si el stack está vacío.
		*/
		inline bool IsEmpty() const {
			return Array.IsEmpty();
		}

		/*
		Devuelve true si se ha resrvado memoria.
		*/
		inline bool HasBeenInitialized() const {
			return Array.HasBeenInitialized();
		}

	private:

		DynamicArray<T> Array;

	};

}

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
