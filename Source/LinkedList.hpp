#pragma once

#include <corecrt_memory.h>
#include <malloc.h>
#include <string>
#include <type_traits>

#include "Assert.h"

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

namespace OSK {

	/// @deprecated No testeado / usado lo suficiente.
	template <typename T> class LinkedList {

	public:

		using TSize = unsigned int;

		/// <summary>
		/// Un nodo contiene un valor almacenado,
		/// y referencias a los nodos anterior y posterior.
		/// </summary>
		class Node {

			friend class LinkedList;

		public:

			/// <summary>
			/// Crea un nodo con el elemento dado.
			/// </summary>
			Node(T&& element) : value(std::move(element)) {

			}

			/// <summary>
			/// Crea un nodo con el elemento dado.
			/// </summary>
			Node(const T& element) requires std::is_copy_assignable_v<T>  : value(element) {

			}

			/// <summary>
			/// Destruye el nodo y el elemento.
			/// </summary>
			~Node() {

			}

			/// <summary>
			/// Crea un nodo con el elemento dado.
			/// </summary>
			T& GetValue() {
				return value;
			}

			/// <summary>
			/// Obtiene el siguiente nodo en la lista.
			/// </summary>
			Node* GetNext() const {
				return next;
			}

			/// <summary>
			/// Obtiene el anterior nodo de la lista.
			/// </summary>
			Node* GetPrevious() const {
				return previous;
			}

			/// <summary>
			/// Comprueba si este nodo es el primero de la lista.
			/// </summary>
			bool IsFirst() const {
				return previous == nullptr;
			}

			/// <summary>
			/// Comprueba si este nodo es el último de la lista.
			/// </summary>
			bool IsLast() const {
				return next == nullptr;
			}

		private:

			/// <summary>
			/// Nodo anterior en la lista.
			/// </summary>
			Node* previous = nullptr;

			/// <summary>
			/// Valor almacenado en el nodo.
			/// </summary>
			T value;

			/// <summary>
			/// Siguiente nodo en la lista.
			/// </summary>
			Node* next = nullptr;

		};

		/// <summary>
		/// Iterador de una linked list.
		/// </summary>
		class Iterator {

		public:

			/// <summary>
			/// Crea el iterador.
			/// </summary>
			Iterator(const Node* node) : node(const_cast<Node*>(node)) {

			}

			/// <summary>
			/// Avaza el iterador una posición.
			/// </summary>
			Iterator& operator++ () {
				if (node)
					node = node->GetNext();

				return *this;
			}

			/// <summary>
			/// Retrocede el iterador una posición.
			/// </summary>
			Iterator& operator-- () {
				if (node)
					node = node->GetPrevious();

				return *this;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator== (const Iterator& other) const {
				return node == other.node;
			}

			/// <summary>
			/// Compara dos iteradores.
			/// </summary>
			bool operator!= (const Iterator& other) const {
				return node != other.node;
			}

			/// <summary>
			/// Devuelve el valor apuntado por el iterador.
			/// </summary>
			/// <returns></returns>
			T& operator* () {
				return node->GetValue();
			}

			/// <summary>
			/// Devuelve el nodo al que apunta el iterador.
			/// </summary>
			Node* GetNode() const {
				return node;
			}

			/// <summary>
			/// Devuelve true si el iterador apunta a un elemento válido.
			/// </summary>
			bool HasValue() const {
				return node != nullptr;
			}

		private:

			/// <summary>
			/// Nodo de la lista.
			/// </summary>
			Node* node = nullptr;

		};

#pragma region Constructores y destructores.

		/// <summary>
		/// Lista vacía.
		/// </summary>
		LinkedList() {
			first = nullptr;
			last = nullptr;
			size = 0;
		}

		/// <summary>
		/// Copia los contenidos de la otra lista.
		/// </summary>
		LinkedList(const LinkedList& other) {
			first = nullptr;
			last = nullptr;
			size = 0;

			InitialCopyFrom(other);
		}

		/// <summary>
		/// Crea el dynamic array con los elementos dados.
		/// </summary>
		LinkedList(const std::initializer_list<T>& list) {
			for (auto& i : list)
				Insert(i);
		}

		/// <summary>
		/// Copia los contenidos de la otra lista.
		/// </summary>
		LinkedList& operator=(const LinkedList& other) {
			if (&other == this)
				return *this;

			CopyFrom(other);

			return *this;
		}

		/// <summary>
		/// Mueve los contenidos de la otra lista a esta lista.
		/// </summary>
		LinkedList(LinkedList&& other) {
			first = nullptr;
			last = nullptr;
			size = 0;

			for (LinkedList<T>::Iterator it = other.begin(); it != other.end(); ++it)
				this->InsertMove(std::move(it.GetNode()->value));

			other.Free();
		}

		/// <summary>
		/// Mueve los contenidos de la otra lista a esta lista.
		/// </summary>
		LinkedList& operator=(LinkedList&& other) {
			for (LinkedList<T>::Iterator it = other.begin(); it != other.end(); ++it)
				this->InsertMove(std::move(it.GetNode()->value));

			other.Free();

			return *this;
		}

		/// <summary>
		/// Destruye el stack.
		/// </summary>
		~LinkedList() {
			Free();
		}

		/// <summary>
		/// Copia los contenidos de la otra lista.
		/// </summary>
		void CopyFrom(const LinkedList& other) {
			Free();

			for (auto& i : other)
				this->Insert(i);
		}

#pragma endregion

#pragma region Funciones

		/// <summary>
		/// Añade un elemento a la lista.
		/// </summary>
		void Insert(const T& value) requires std::is_copy_assignable_v<T> {
			Node* node = new Node(value);

			if (last == nullptr) {
				first = node;
				last = node;
			}
			else {
				last->next = node;
				node->previous = last;

				last = node;
			}

			size++;
		}

		/// <summary>
		/// Añade un elemento a la lista, en la posición dada.
		/// El elemento añadido toma la posición dada, y el
		/// elemento que antes estaba en la posición, avanza
		/// a la posicion + 1.
		/// </summary>
		void InsertAt(const T& value, TSize position) {
			if (position == 0 && size == 0)
				Insert(value);

			Node* targetNode = GetNodeFromIndex(position);

#ifdef OSK_SAFE
			OSK_ASSERT(targetNode != nullptr, "Se ha intentado insertar en la posición " + std::to_string(position) + ", pero no hay elementos en la posición.");
#endif

			size++;

			if (position == 0) {
				if (size == 0) {
					Insert(value);

					return;
				}

				Node* prevFirst = first;
				first = new Node(value);
				prevFirst->previous = first;

				return;
			}

			Node* previous = targetNode->GetPrevious();
			Node* newNode = new Node(value);

			previous->next = newNode;
			newNode->previous = previous;

			targetNode->previous = newNode;
			newNode->next = targetNode;
		}

		/// <summary>
		/// Añade un elemento a la lista.
		/// </summary>
		void Insert(T&& value) {
			Node* node = new Node(std::move(value));

			if (last == nullptr) {
				first = node;
				last = node;
			}
			else {
				last->next = node;
				node->previous = last;

				last = node;
			}

			size++;
		}

		/// <summary>
		/// Añade un elemento a la lista.
		/// </summary>
		void InsertMove(T&& value) {
			Node* node = new Node(std::move(value));

			if (last == nullptr) {
				first = node;
				last = node;
			}
			else {
				last->next = node;
				node->previous = last;

				last = node;
			}

			size++;
		}

		/// <summary>
		/// Añade un elemento a la lista, en la posición dada.
		/// El elemento añadido toma la posición dada, y el
		/// elemento que antes estaba en la posición, avanza
		/// a la posicion + 1.
		/// </summary>
		void InsertAt(T&& value, TSize position) {
			if (position == 0 && size == 0)
				Insert(std::move(value));

			Node* targetNode = GetNodeFromIndex(position);

#ifdef OSK_SAFE
			OSK_ASSERT(targetNode != nullptr, "Se ha intentado insertar en la posición " + std::to_string(position) + ", pero no hay elementos en la posición.");
#endif

			size++;

			if (position == 0) {
				if (size == 0) {
					Insert(std::move(value));

					return;
				}

				Node* prevFirst = first;
				first = new Node(std::move(value));
				prevFirst->previous = first;

				return;
			}

			Node* previous = targetNode->GetPrevious();
			Node* newNode = new Node(std::move(value));

			previous->next = newNode;
			newNode->previous = previous;

			targetNode->previous = newNode;
			newNode->next = targetNode;
		}

		/// <summary>
		/// Devuevle el elemento que está en la posición 'i', desde el primer elemento.
		/// </summary>
		const T& At(TSize i) const {
			Node* node = GetNodeFromIndex(i);
#ifdef OSK_SAFE
			OSK_ASSERT(!IsEmpty(), "La lista está vacía.");
			OSK_ASSERT(node != nullptr, "Se ha intentado acceder al elemento " + std::to_string(i) + ", pero solo hay " + std::to_string(GetSize()) + " elementos.");
#endif

			return node->GetValue();
		}
		T& At(TSize i) {
			Node* node = GetNodeFromIndex(i);
#ifdef OSK_SAFE
			OSK_ASSERT(!IsEmpty(), "La lista está vacía.");
			OSK_ASSERT(node != nullptr, "Se ha intentado acceder al elemento " + std::to_string(i) + ", pero solo hay " + std::to_string(GetSize()) + " elementos.");
#endif

			return node->GetValue();
		}

		/// <summary>
		/// Devuevle el elemento que está en la posición 'i', desde el primer elemento.
		/// </summary>
		T& operator[] (TSize i) {
			return At(i);
		}

		/// <summary>
		/// Elimina el elemento en la posición dada.
		/// </summary>
		void RemoveAt(TSize position) {
			Node* node = GetNodeFromIndex(position);

			if (node == nullptr) {
#ifdef OSK_SAFE
				OSK_ASSERT(false, "Se ha intentado eliminar al elemento en la posición " + std::to_string(position) + ", pero solo hay " + std::to_string(GetSize()) + " elementos.");
#else
				return;
#endif
			}

			Node* previous = node->previous;
			Node* next = node->next;

			if (previous)
				previous->next = node->next;

			if (next)
				next->previous = node->previous;

			delete node;

			size--;

			if (position == 0 && !IsEmpty())
				first = next;

			if (next == nullptr)
				last = previous;

			if (IsEmpty())
				first = nullptr;
		}

		/// <summary>
		/// Elimina el elemento dado, si existe.
		/// </summary>
		void Remove(const T& value) {
			Node* it = first;
			for (TSize i = 0; i < GetSize(); i++) {
				if (it->GetValue() == value) {
					RemoveAt(i);

					return;
				}

				it = it->GetNext();
			}

#ifdef OSK_SAFE
			//OSK_ASSERT(false, "Error: no se ha encontrado el elemento.");
#endif
		}

		/// <summary>
		/// Añade los elementos de la otra lista a esta lista.
		/// </summary>
		void InsertAll(const LinkedList& other) {
			for (auto& i : other)
				Insert(i);
		}

		/// <summary>
		/// Elimina todos los elementos de la lista.
		/// </summary>
		void Free() {
			while (!IsEmpty())
				RemoveAt(0);

			first = nullptr;
			last = nullptr;
		}

		Iterator Find(const T& elem) {
			auto it = begin();
			while (it != end()) {
				if (*it == elem)
					return it;
			}

			return end();
		}

		void Push(const T& elem) {
			Insert(elem);
		}
		void Push(T&& elem) {
			Insert(std::move(elem));
		}
		T Pop() {
			T output = last->GetValue();

			RemoveLast();

			return output;
		}
		T& Peek() {
			return last->GetValue();
		}
		void RemoveLast() {
			RemoveAt(size - 1);
		}

		void Enqueue(const T& elem) {
			Insert(elem);
		}
		void Enqueue(T&& elem) {
			Insert(std::move(elem));
		}
		T Dequeue() {
			T output = first->GetValue();

			RemoveAt(0);

			return output;
		}

		bool ContainsElement(const T& elem) const {
			return Find(elem) != end();
		}

#pragma endregion

#pragma region Getters & setters

		/// <summary>
		/// Devuelve el número de elementos en el stack.
		/// </summary>
		TSize GetSize() const {
			return size;
		}

		/// <summary>
		/// Devuelve true si no hay ningún elemento en el stack.
		/// </summary>
		bool IsEmpty() const {
			return size == 0;
		}

#pragma endregion

#pragma region Iteradores

		/// <summary>
		/// Iterador que apunta al inicio de la lista.
		/// </summary>
		Iterator begin() const {
			return Iterator(first);
		}

		/// <summary>
		/// Iterador final.
		/// </summary>
		Iterator end() const {
			return Iterator(nullptr);
		}

		/// <summary>
		/// Devuelve un iterador que apunta al elemento dado.
		/// Si el elemento no está en la lista, el iterardor
		/// no tiene valor interno.
		/// </summary>
		Iterator Find(const T& value) const {
			Node* node = first;

			for (TSize i = 0; i < GetSize(); i++) {
				if (node->GetValue() == value)
					return Iterator(node);

				node = node->next;
			}

			return Iterator(nullptr);
		}

#pragma endregion

	private:

		/// <summary>
		/// Copia los contenidos de la otra lista.
		/// </summary>
		void InitialCopyFrom(const LinkedList& other) {
			for (auto& i : other)
				this->Insert(i);
		}

		/// <summary>
		/// Obtiene el nodo en la posición dada.
		/// Si no hay nodo en la posición, devuelve nullptr.
		/// </summary>
		Node* GetNodeFromIndex(TSize id) const {
			Node* node = first;

			for (TSize i = 0; i < id; i++) {
				if (node)
					node = node->GetNext();
				else
					break;
			}

			return node;
		}

		/// <summary>
		/// Primer nodo.
		/// </summary>
		Node* first = nullptr;

		/// <summary>
		/// Último nodo.
		/// </summary>
		Node* last = nullptr;

		/// <summary>
		/// Número de elementos totales.
		/// </summary>
		TSize size = 0;

	};

	template <typename T> using LinkedStack = LinkedList<T>;
	template <typename T> using Queue = LinkedList<T>;

}
