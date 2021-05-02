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

	/// <summary>
	/// Un stack que usa un array por dentro.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T> class ArrayStack {

	public:
				
		/// <summary>
		/// Crea un stack sin inicializar.
		/// </summary>
		ArrayStack() {

		}

		/// <summary>
		/// Crea un stack con capacidad para <numberOfElements>.
		/// </summary>
		/// <param name="numberOfElements">Capacidad del stack.</param>
		ArrayStack(size_t numberOfElements) {
			Allocate(numberOfElements);
		}

		/// <summary>
		/// Crea un stack copiando los elementos de <stack>.
		/// </summary>
		/// <param name="stack">Stack a copiar.</param>
		ArrayStack(const ArrayStack& stack) {
			CopyFrom(stack);
		}

		/// <summary>
		/// Copia el stack.
		/// </summary>
		/// <param name="stack">Stack a copiar.</param>
		/// <returns>Self.</returns>
		ArrayStack& operator=(const ArrayStack& stack) {
			if (&stack == this)
				return *this;

			CopyFrom(stack);
		}

		/// <summary>
		/// Destruye el stack.
		/// </summary>
		~ArrayStack() {
			Free();
		}

		/// <summary>
		/// Reserva memoria para <maxElements> elementos.
		/// </summary>
		/// <param name="maxElements">Número máximo de elementos en el stack.</param>
		void Allocate(size_t maxElements) {
			if (HasBeenInitialized()) {
				Free();
			}

			Data = (T*)malloc(maxElements * sizeof(T));
			MaxSize = maxElements;
		}

		/// <summary>
		/// Introduce un elemento al stack.
		/// </summary>
		/// <param name="element">Elemento a insertar.</param>
		void Push(const T& element) {
#ifdef OSK_DS_SAFE_MODE
			if (IsFull()) {
				throw std::runtime_error("ERROR: stack is full.");
			}
#endif
			Data[CurrentSize] = element;
			CurrentSize++;
		}

		/// <summary>
		/// Devuelve el último elemento que se haya añadido.
		/// </summary>
		/// <returns>Último elemento del stack.</returns>
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

		/// <summary>
		/// Elimina los elementos, sin liberar memoria. No llama a los destructores.
		/// </summary>
		void Clear() {
			CurrentSize = 0;
		}

		/// <summary>
		/// Copia los elementos del stack.
		/// </summary>
		/// <param name="stack">Stack a copiar.</param>
		void CopyFrom(const ArrayStack& stack) {
			Free();

			Allocate(stack.MaxSize);
			memcpy(Data, stack.Data, sizeof(T) * MaxSize);
		}

		/// <summary>
		/// Libera memoria. No llama a los destructores.
		/// </summary>
		void Free() {
			if (HasBeenInitialized()) {
				free(Data);
				Data = nullptr;
			}

			CurrentSize = 0;
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados en el stack.
		/// </summary>
		/// <returns>Elementos en el stack.</returns>
		inline size_t GetSize() const {
			return CurrentSize;
		}

		/// <summary>
		/// Devuelve el número máximo de elementos que se pueden almacenar.
		/// </summary>
		/// <returns>Número máximo de elementos.</returns>
		inline size_t GetMaxSize() const {
			return MaxSize;
		}

		/// <summary>
		/// Devuelve true si el stack está vacío.
		/// </summary>
		/// <returns>Estado del stack.</returns>
		inline bool IsEmpty() const {
			return GetSize() == 0;
		}

		/// <summary>
		/// Devuelve true si el stack está lleno.
		/// </summary>
		/// <returns>Estado del stack.</returns>
		inline bool IsFull() const {
			return GetSize() == GetMaxSize();
		}

		/// <summary>
		/// Devuelve true si se ha resrvado memoria.
		/// </summary>
		/// <returns>Estado del stack.</returns>
		inline bool HasBeenInitialized() const {
			return Data != nullptr;
		}

	private:

		/// <summary>
		/// Array.
		/// </summary>
		T* Data = nullptr;
				
		/// <summary>
		/// Máximo número de elementos.
		/// </summary>
		size_t MaxSize = 0;

		/// <summary>
		/// Número de elementos guardados actualmente.
		/// </summary>
		size_t CurrentSize = 0;

	};


	/// <summary>
	/// Representa un stack. Implementado usando DynamicArray.
	/// </summary>
	/// <typeparam name="T">Tipo de objetos en el stack.</typeparam>
	template <typename T> class Stack {

	public:

		/// <summary>
		/// Introduce un elemento al stack.
		/// </summary>
		/// <param name="element">Elemento a insertar.</param>
		void Push(const T& element) {
			Array.Insert(element);
		}

		/// <summary>
		/// Devuelve el último elemento que se haya añadido.
		/// </summary>
		/// <returns>Último elemento del stack.</returns>
		T Pop() {
			T value = Array.At(Array.GetSize() - 1);
			Array.RemoveLast();
			return value;
		}

		/// <summary>
		/// Copia los elementos del stack.
		/// </summary>
		/// <param name="stack">Stack a copiar.</param>
		void CopyFrom(const Stack& stack) {
			Array.CopyContentFrom(stack.Array);
		}

		/// <summary>
		/// Libera memoria. No llama a los destructores.
		/// </summary>
		void Free() {
			Array.Free();
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados en el stack.
		/// </summary>
		/// <returns>Elementos en el stack.</returns>
		inline size_t GetSize() const {
			return Array.GetSize();
		}

		/// <summary>
		/// Devuelve true si el stack está vacío.
		/// </summary>
		/// <returns>Estado el stack.</returns>
		inline bool IsEmpty() const {
			return Array.IsEmpty();
		}

		/// <summary>
		/// Devuelve true si se ha resrvado memoria.
		/// </summary>
		/// <returns>Estado el stack.</returns>
		inline bool HasBeenInitialized() const {
			return Array.HasBeenInitialized();
		}

	private:

		/// <summary>
		/// Array.
		/// </summary>
		DynamicArray<T> Array;

	};

}

#ifdef OSK_DS_SAFE_MODE
#undef OSK_DS_SAFE_MODE
#endif // OSK_DS_SAFE_MODE
