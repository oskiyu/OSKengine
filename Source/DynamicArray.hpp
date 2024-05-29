#pragma once

#include "BadAllocException.h"
#include "NumericTypes.h"

#include <corecrt_memory.h>
#include <malloc.h>
#include <string>
#include <memory>
#include <limits>
#include <initializer_list>
#include <span>

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

#ifndef OSK_CUSTOM_DARRAY
#define OSK_CUSTOM_DARRAY
#endif
#ifdef OSK_CUSTOM_DARRAY

	/// @brief Factor de crecimiento.
	/// @see GrowthFactorType
	constexpr static unsigned int DynamicArrayGrowthFactor = 2;

	/// @brief Comportamiento del array cuando no hay espacio:
	/// -EXPONENTIAL: se amplia de manera exponencial (Capacity * Factor).
	///	-LINEAL: se amplia de manera lineal (Capacity + Factor).
	enum class GrowthFactorType {

		/// @brief Se amplia de manera exponencial (Capacity * Factor).
		EXPONENTIAL,

		/// @brief Se amplia de manera lineal (Capacity + Factor).
		LINEAL

	};


	namespace _DArr {
		template <typename T_>
		concept IsCopyable = std::copyable<T_> || std::is_copy_assignable_v<T_>;

		template <typename T_>
		concept IsMovable = std::movable<T_> || std::is_move_assignable_v<T_>;
	}


	/// @brief Dynamic array: array que puede cambiar de tamaño.
	/// @tparam T Tipo de elementos que van a ser almacenados.
	/// @note Usa memoria contígua.
	/// @warning No ofrece estabilidad de punteros.
	template <typename T> requires ::std::copyable<T> || ::std::movable<T>
	class DynamicArray {

	public:

		/// @brief Tamaño inicial.
		static constexpr USize64 INITIAL_RESERVE_SIZE = 10uL;

#ifndef OSK_DARRAY_CLASS_ITER
#define OSK_DARRAY_CLASS_ITER
#endif // !OSK_DARRAY_CLASS_ITER

#ifdef OSK_DARRAY_CLASS_ITER

		/// @brief Clase que representa un elemento de un DynamicArray.
		/// @pre La colección generadora debe tener estabilidad de puntero mientras el iterador
		/// siga vivo.
		/// @warning Si se modifica el dynamic array original, queda en un estado inválido.
		/// @warning Si se modifica la localización en memoria del dynamic array original, queda en un estado inválido.
		class Iterator {

			friend class DynamicArray;

		public:

			/// @brief Crea el iterador.
			/// @param i Índice del elemento.
			/// @param arr Puntero al array.
			constexpr explicit Iterator(T* ptr) noexcept : m_pointer(ptr) { }

			/// @brief Hace que el iterador apunte al elemento siguiente.
			/// @return Self (apuntando al elemento anterior).
			constexpr Iterator operator++ () noexcept {
				m_pointer++;
				return *this;
			}

			/// @brief Hace que el iterador apunte al elemento anterior.
			/// @return Self (apuntando al elemento anterior).
			constexpr Iterator operator-- () noexcept {
				m_pointer--;
				return *this;
			}

			constexpr auto operator<=>(const Iterator&) const noexcept = default;

			T& operator*() { return *m_pointer; }
			T* GetPtr() { return m_pointer; }

			/// @return True si el iterator es vacío:
			/// no apunta a un elemento válido.
			constexpr bool IsEmpty() const noexcept { return m_pointer == nullptr; }

		private:

			/// @brief Array al que pertenece.
			T* m_pointer = nullptr;

		};

		/// @brief Clase que representa un elemento de un DynamicArray.
		/// @pre La colección generadora debe tener estabilidad de puntero mientras el iterador
		/// siga vivo.
		/// @warning Si se modifica el dynamic array original, queda en un estado inválido.
		/// @warning Si se modifica la localización en memoria del dynamic array original, queda en un estado inválido.
		class ConstIterator {

			friend class DynamicArray;

		public:

			/// @brief Crea el iterador.
			/// @param i Índice del elemento.
			/// @param arr Puntero al array.
			constexpr ConstIterator(const T* ptr) noexcept : m_pointer(ptr) { }

			/// @brief Hace que el iterador apunte al elemento siguiente.
			/// @return Self (apuntando al elemento anterior).
			constexpr ConstIterator operator++ () noexcept {
				m_pointer++;
				return *this;
			}

			/// @brief Hace que el iterador apunte al elemento anterior.
			/// @return Self (apuntando al elemento anterior).
			constexpr ConstIterator operator-- () noexcept {
				m_pointer--;
				return *this;
			}

			constexpr auto operator<=>(const ConstIterator&) const noexcept = default;

			constexpr const T& operator*() const { return *m_pointer; }
			constexpr const T* GetPtr() const { return m_pointer; }

			/// @return True si el iterator es vacío:
			/// no apunta a un elemento válido.
			constexpr bool IsEmpty() const noexcept { return m_pointer == nullptr; }

		private:

			/// @brief Array al que pertenece.
			const T* m_pointer = nullptr;

		};

#else

		using Iterator = T*;
		using ConstIterator = const T*;

#endif

#pragma region Constructores y destructores.

		/// @brief Crea el dynamic array con espacio inicial para 10 elementos.
		DynamicArray() : m_capacity(INITIAL_RESERVE_SIZE)  {
			m_data = (T*)malloc(sizeof(T) * m_capacity);
		}

		/// @brief Crea el dynamic array con los elementos dados.
		/// @param list Elementos iniciales.
		DynamicArray(const std::initializer_list<T>& list) requires _DArr::IsCopyable<T> : m_count(list.size()), m_capacity(list.size()) {
			m_data = (T*)malloc(sizeof(T) * m_capacity);

			UIndex64 index = 0uL;
			for (const auto& i : list) {
				new(::std::addressof(m_data[index])) T(i);
				index++;
			}
		}

		/// @brief Constructor de copia.
		/// @param arr Otro DynamicArray.
		DynamicArray(const DynamicArray& arr) requires _DArr::IsCopyable<T> : m_count(arr.m_count), m_capacity(arr.m_count) {
			m_data = (T*)malloc(sizeof(T) * m_capacity);

			for (USize64 i = 0; i < m_count; i++) {
				new(::std::addressof(m_data[i])) T(arr.m_data[i]);
			}
			
			growthFactorType = arr.growthFactorType;
		}


		
		/// @brief Copia el array.
		/// @param arr Otro DynamicArray.
		/// @return DynamicArray copiado.
		DynamicArray& operator=(const DynamicArray& arr) requires _DArr::IsCopyable<T> {
			if (&arr == this)
				return *this;

			Free();
			Allocate(arr.m_capacity);

			growthFactorType = arr.growthFactorType;

			for (UIndex64 i = 0; i < arr.m_count; i++)
				new (::std::addressof(m_data[i])) T(arr.m_data[i]);
			m_count = arr.m_count;

			return *this;
		}

		/// @brief Transfirere el contenido de @p arr a este array.
		/// @param arr Otro array.
		/// @warning Deja a @p arr en un estado inválido.
		DynamicArray(DynamicArray&& arr) noexcept : m_capacity(arr.m_count), m_count(arr.m_count)  {
			m_data = (T*)malloc(sizeof(T) * m_capacity);
			
			for (UIndex64 i = 0; i < arr.m_count; i++) {
				if constexpr (_DArr::IsMovable<T>) {
					new (::std::addressof(m_data[i])) T(std::move(arr.m_data[i]));
				}
				else {
					new (::std::addressof(m_data[i])) T(arr.m_data[i]);
				}
			}

			growthFactorType = arr.growthFactorType;
		}

		/// @brief Transfirere el contenido de 'arr' a este array.
		/// @param arr Otro array.
		/// @warning Deja a @p arr en un estado inválido.
		DynamicArray& operator=(DynamicArray&& arr) {
			if (&arr == this)
				return *this;

			Free();
			Allocate(arr.m_capacity);

			growthFactorType = arr.growthFactorType;

			for (UIndex64 i = 0; i < arr.GetSize(); i++) {
				if constexpr  (_DArr::IsMovable<T>) {
					InsertMove(::std::move(arr.m_data[i]));
				}
				else if constexpr (_DArr::IsCopyable<T>) {
					InsertCopy(arr.m_data[i]);
				}
			}

			arr.Free();

			return *this;
		}

		/// @brief Destruye el array.
		~DynamicArray() {
			Free ();
		}
		

		/// @brief Crea un DynamicArray con espacio reservado para el número
		/// de elementos dado.
		/// @param reservedSize Tamaño reservado.
		static DynamicArray CreateReservedArray(USize64 reservedSize) {
			DynamicArray output;
			output.Reserve(reservedSize);

			return output;
		}

		/// @brief Crea un DynamicArray con un número de elementos ya creados e insertados.
		/// @tparam ...TArgs Argumentos para el constructor de los elementos.
		/// @param size Número de elementos.
		/// @param ...args Argumentos para el constructor de los elementos.
		/// @return DynamicArray con @p size elementos iguales.
		template <typename... TArgs> 
		static DynamicArray CreateResizedArray(USize64 size, TArgs... args) requires _DArr::IsCopyable<T> {
			DynamicArray output;
			output.Resize(size, args...);

			return output;
		}

		/// @brief Crea un DynamicArray con un número de elementos ya creados e insertados.
		/// @tparam ...TArgs Argumentos para el constructor de los elementos.
		/// @param size Número de elementos.
		/// @param ...args Argumentos para el constructor de los elementos.
		/// @return DynamicArray con @p size elementos iguales.
		template <typename... TArgs> 
		static DynamicArray CreateResizedArrayMove(USize64 size, TArgs&&... args) requires _DArr::IsMovable<T> {
			DynamicArray output;
			output.ResizeMove(size, args...);

			return output;
		}

#pragma endregion

#pragma region Funciones

		/// @brief Reserva espacio para 'size' elementos.
		/// Preserva los elementos anteriores.
		/// @param size Número final de elementos.
		/// @throws BadAllocException Si no se puede reservar memoria.
		void Allocate(USize64 count) {
			m_capacity = count;

			T* previousData = m_data;
			m_data = m_data == nullptr
				? (T*)malloc(sizeof(T) * count)
				: (T*)realloc(m_data, sizeof(T) * count);

			if (count > 0 && !m_data)
				throw BadAllocException();
		}

		/// @brief Inserta una copia del elemento al final del array.
		/// @param element Elemento a copiar.
		/// @pre El elemento es copiable.
		void InsertCopy(const T& element) requires _DArr::IsCopyable<T> {
			EnsureSpace(m_count + 1);

			new (::std::addressof(m_data[m_count])) T(element);

			m_count++;
		}


		/// @brief Inserta un elemento al final del array.
		/// @param element Elemento añadido.
		void Insert(const T& element) requires _DArr::IsCopyable<T> {
			InsertCopy(element);
		}

		/// @brief Inserta un elemento al final del array.
		/// @param element Elemento añadido.
		void InsertMove(T&& element) requires _DArr::IsMovable<T> {
			EnsureSpace(m_count + 1);

			new (::std::addressof(m_data[m_count])) T(::std::move(element));

			m_count++;
		}

		/// <summary>
		/// Inserta un elemento al final del array.
		/// </summary>
		void Insert(T&& element) requires _DArr::IsMovable<T> {
			InsertMove(::std::move(element));
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		/// 
		/// @throws std::runtime_error Si el índice es inválido.
		/// @throws std::runtime_error Si no ha sido correctamente inicializado.
		const T& At(UIndex64 index) const {
#ifdef OSK_SAFE
			// OSK_ASSERT(index < capacity, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index < size, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(size) + " elements.");
			// OSK_ASSERT(index >= 0, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to access element from an uninitialized array.");
#endif
			return m_data[index];
		}
		const T& ConstAt(UIndex64 index) const {
#ifdef OSK_SAFE
			// OSK_ASSERT(index < capacity, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index < size, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(size) + " elements.");
			// OSK_ASSERT(index >= 0, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to access element from an uninitialized array.");
#endif
			return m_data[index];
		}
		T AtCpy(UIndex64 index) const requires _DArr::IsCopyable<T> {
#ifdef OSK_SAFE
			// OSK_ASSERT(index < capacity, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index < size, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(size) + " elements.");
			// OSK_ASSERT(index >= 0, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to access element from an uninitialized array.");
#endif
			return m_data[index];
		}
		T& At(UIndex64 index) {
#ifdef OSK_SAFE
			// OSK_ASSERT(index < capacity, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index < size, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(size) + " elements.");
			// OSK_ASSERT(index >= 0, "DynamicArray: tried to access element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to access element from an uninitialized array.");
#endif
			return m_data[index];
		}

		/// <summary>
		/// Añade el elemento al final de la lista.
		/// </summary>
		void Push(const T& elem) requires _DArr::IsCopyable<T> {
			InsertCopy(elem);
		}

		/// <summary>
		/// Añade el elemento al final de la lista.
		/// </summary>
		void Push(T&& elem) requires _DArr::IsMovable<T> {
			InsertMove(::std::move(elem));
		}

		/// <summary>
		/// Devuelve el último elemento de la lista. 
		/// 
		/// @note No lo quita de la lista.
		/// </summary>
		T& Peek() {
			return At(m_count - 1);
		}

		/// <summary>
		/// Devuelve el último elemento de la lista. 
		/// 
		/// @note Lo quita de la lista.
		/// </summary>
		T Pop() requires _DArr::IsMovable<T> {
			T output = At(m_count - 1);

			RemoveLast();

			return output;
		}

		/// <summary>
		/// Añade los elementos de 'arr' a este array.
		/// </summary>
		/// <param name="arr">Elementos a añadir.</param>
		/// 
		/// @bug No llama a los constructores de copia.
		void InsertAll(const DynamicArray& arr) requires _DArr::IsCopyable<T> {
			EnsureSpace(m_count + arr.m_count);

			for (const T& i : arr)
				InsertCopy(i);
		}

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// </summary>
		inline void Reserve(USize64 size) {
			Allocate(size);
		}

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// Se añaden elementos hasta que haya 'size' elementos.
		/// </summary>
		template <typename... TArgs> inline void Resize(USize64 size, const TArgs&... args) {
			const auto oldCount = this->m_count;
			Reserve(size);

			for (UIndex64 i = oldCount; i < size; i++)
				new (::std::addressof(m_data[i])) T(args...);

			this->m_count = size;
		}

		/// <summary>
		/// Cambia el tamaño del array.
		/// Los datos se conservan, siempre que se pueda.
		/// Se añaden elementos hasta que haya 'size' elementos.
		/// </summary>
		template <typename... TArgs> inline void ResizeMove(USize64 size, TArgs&&... args) {
			const auto oldSize = this->m_count;
			Reserve(size);

			for (const UIndex64 i = oldSize; i < size; i++)
				new (::std::addressof(m_data[i])) T(args...);

			this->m_count = size;
		}

		/// <summary>
		/// Elimina el elemento en la posición dada.
		/// Llama al destructor.
		/// </summary>
		/// 
		/// @throws std::runtime_error Si el índice es inválido.
		/// @throws std::runtime_error Si no ha sido correctamente inicializado.
		void RemoveAt(UIndex64 index) {
#ifdef OSK_SAFE
			// OSK_ASSERT(index < capacity, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index < size, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(index >= 0, "DynamicArray: tried to remove element number " + std::to_string(index) + ", but there are only " + std::to_string(capacity) + " reserved elements.");
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to remove element from an uninitialized array.");
#endif // _DEBUG

			m_data[index].~T();

			for (UIndex64 i = index; i < m_count; i++) {
				if constexpr (_DArr::IsMovable<T>) {
					new (::std::addressof(m_data[i])) T(::std::move(m_data[i + 1]));
				}
				else {
					new (::std::addressof(m_data[i])) T(m_data[i + 1]);
				}
			}

			m_count--;
		}

		/// <summary>
		/// Elimina el eleemento dado de la lista.
		/// </summary>
		/// 
		/// @pre El tipo del elemento debe tener definido el operador de comparación '=='.
		/// 
		/// @note Si no se encuentra ningún elemento, no ocurre nada.
		/// @note Elimina únicamente la primera aparición.
		void Remove(const T& elem) {
			for (UIndex64 i = 0; i < m_count; i++)
				if (m_data[i] == elem)
					RemoveAt(i);
		}
			
		/// <summary>
		/// Elimina el último elemento del array.
		/// </summary>
		/// 
		/// @throws std::runtime_error Si el array está vacío.
		void RemoveLast() {
#ifdef OSK_SAFE
			// OSK_ASSERT(HasBeenInitialized(), "DynamicArray: tried to remove element from an uninitialized array.");
#endif // _DEBUG
			m_data[m_count - 1].~T();

			m_count--;
		}

		/// <summary>
		/// Comprueba si el elemento está presente en el array.
		/// </summary>
		/// 
		/// @pre El tipo del elemento debe tener definido el operador de comparación '=='.
		bool ContainsElement(const T& elem) const {
			return !Find(elem).IsEmpty();
		}

		/// <summary>
		/// Obtiene el índice del primer elemento almacenado que sea igual
		/// a el dado.
		/// </summary>
		/// 
		/// @pre El tipo del elemento debe tener definido el operador de comparación '=='.
		/// 
		/// @note Si no se encuentra, devuelve std::numeric_limits<USize64>::max().
		UIndex64 GetIndexOf(const T& elem) const {
			const ConstIterator it = Find(elem);

			return it.GetPtr()
				? static_cast<USize64>(it.GetPtr() - m_data) / sizeof(T)
				: std::numeric_limits<UIndex64>::max();
		}

		constexpr static UIndex64 NotFoundIndex = std::numeric_limits<UIndex64>::max();

		/// <summary>
		/// Obtiene el iterador del primer elemento almacenado que sea igual
		/// a el dado.
		/// </summary>
		/// 
		/// @note Devuelve un iterador vacío si no está.
		/// 
		/// @pre El tipo del elemento debe tener definido el operador de comparación '=='.
		Iterator Find(const T& elem) {
			for (USize64 i = 0; i < m_count; i++)
				if (elem == m_data[i])
					return Iterator(m_data + i);

			return Iterator(nullptr);
		}

		ConstIterator Find(const T& elem) const {
			for (USize64 i = 0; i < m_count; i++)
				if (elem == m_data[i])
					return ConstIterator(m_data + i);

			return ConstIterator(nullptr);
		}

		/// <summary>
		/// Elimina el array, liberando memoria.
		/// </summary>
		void Free() {
			if (m_data) {
				Empty();

				free(m_data);
				m_data = nullptr;
			}

			m_capacity = 0u;
			m_count = 0u;
		}

		/// <summary>
		/// Elimina los elementos, sin liberar memoria.
		/// </summary>
		void Empty() {
			for (USize64 i = 0; i < m_count; i++)
				m_data[i].~T();

			m_count = 0;
		}

		/// <summary>
		/// Devuelve el elemento en la posición dada.
		/// </summary>
		/// <param name="i">Posición en el array.</param>
		/// <returns>Elemento.</returns>
		/// 
		/// @throws std::runtime_error Si el índice es inválido.
		/// @throws std::runtime_error Si no ha sido correctamente inicializado.
		const T& operator[] (UIndex64 i) const {
			return At(i);
		}
		T& operator[] (UIndex64 i) {
			return At(i);
		}

#pragma endregion

#pragma region Getters & setters

		/// <summary>
		/// Devuelve true si el array se ha inicializado.
		/// </summary>
		/// <returns>Estado del array.</returns>
		bool HasBeenInitialized() const noexcept {
			return m_data != nullptr;
		}

		/// <summary>
		/// Devuelve true si el array está vacío.
		/// </summary>
		bool IsEmpty() const noexcept {
			return m_count == 0;
		}

		/// <summary>
		/// Devuelve el número de elementos almacenados.
		/// </summary>
		USize64 GetSize() const noexcept {
			return m_count;
		}

		/// <summary>
		/// Devuelve el número de elementos reservados.
		/// </summary>
		USize64 GetReservedSize() const noexcept {
			return m_capacity;
		}

		/// <summary>
		/// Devuelve el array original.
		/// </summary>
		const T* GetData() const noexcept {
			return m_data;
		}
		T* GetData() noexcept {
			return m_data;
		}

		/// @return Span con todos los elementos de la lista.
		std::span<T> GetFullSpan() {
			return std::span<T>(m_data, m_count);
		}

		/// @return Span con todos los elementos de la lista.
		std::span<const T> GetFullSpan() const {
			return std::span<T>(m_data, m_count);
		}

		/// @brief Crea un span con un subrango de elementos.
		/// @param startIndex Índice del primer objeto.
		/// @param count Número de objetos.
		/// @return Span con los objetos indicados.
		/// 
		/// @pre @p startIndex debe ser menor que el número de elementos
		/// de la lista.
		/// @pre @p startIndex + @p count debe ser menor que el número de elementos
		/// de la lista.
		std::span<T> GetSpan(UIndex64 startIndex, USize64 count) {
			return std::span<T>(m_data + startIndex, count);
		}

		/// @brief Crea un span con un subrango de elementos.
		/// @param startIndex Índice del primer objeto.
		/// @param count Número de objetos.
		/// @return Span con los objetos indicados.
		/// 
		/// @pre @p startIndex debe ser menor que el número de elementos
		/// de la lista.
		/// @pre @p startIndex + @p count debe ser menor que el número de elementos
		/// de la lista.
		std::span<const T> GetSpan(UIndex64 startIndex, USize64 count) const {
			return std::span<T>(m_data + startIndex, count);
		}

#pragma endregion

#pragma region Iteradores

		Iterator begin() noexcept {
			return GetIterator(0);
		}
		Iterator end() noexcept {
			return GetIterator(m_count);
		}

		constexpr ConstIterator begin() const noexcept {
			return GetConstIterator(0);
		}
		constexpr ConstIterator end() const noexcept {
			return GetConstIterator(m_count);
		}

		/// <summary>
		/// Devuelve el iterador que apunta a un elemento en particular.
		/// </summary>
		/// 
		/// @note Puede no apuntar a un element válido.
		Iterator GetIterator(UIndex64 index) noexcept {
			return Iterator(m_data + index);
		}

		constexpr ConstIterator GetConstIterator(UIndex64 index) const noexcept {
			return ConstIterator(m_data + index);
		}

#pragma endregion

		/// <summary>
		/// Comportamiento al aumentar de tamaño.
		/// </summary>
		GrowthFactorType growthFactorType = GrowthFactorType::EXPONENTIAL;

	private:

		void EnsureSpace(USize64 newCount) {
			if (!HasBeenInitialized() || m_capacity == 0)
				Allocate(INITIAL_RESERVE_SIZE);

			if (newCount < m_capacity - 1)
				return;

			const USize64 newAllocSize = growthFactorType == GrowthFactorType::EXPONENTIAL
				? m_capacity * DynamicArrayGrowthFactor
				: m_capacity + DynamicArrayGrowthFactor;

			Allocate(newAllocSize);
		}

		/// @brief Array.
		T* m_data = nullptr;

		/// @brief Número de espacios reservados.
		USize64 m_capacity = 0;

		/// @brief Elementos guardados.
		USize64 m_count = 0;

	};

	template <typename T> using ArrayStack = DynamicArray<T>;

#endif

}


#ifdef OSK_SAFE
#undef OSK_SAFE
#endif // OSK_DS_SAFE_MODE
