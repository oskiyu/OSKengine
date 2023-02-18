#ifndef OSK_UNIQUE_PTR
#define OSK_UNIQUE_PTR

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	
	/// @brief Estructura que es due�a de un objeto almacenado en heap.
	/// Al destruirse el UniquePtr, tambi�n se destruir� el objeto
	/// y se liberar� la memoria asociada.
	/// 
	/// @tparam T Tipo de dato del objeto.
	/// 
	/// @note Usarse como miembro de una clase deshabilitar� su constructor 
	/// y su operador de copia.
	/// 
	/// @warning Debe incluirse el tipo que se vaya a usar dentro del UniquePtr.
	/// @code #include "T.h" @endcode
	/// De lo contrario, no se llamar� al destructor al eliminarse.
	template <typename T> /*requires std::is_destructible_v<T>*/ class UniquePtr {

	public:

		/// @brief Crea un UniquePtr vac�o.
		UniquePtr() : pointer(nullptr) {

		}

		/// @brief Crea un UniquePtr a partir del objeto almacenado en la
		/// direcci�n de memoria dada.
		/// @param data Puntero al objeto almacenado en memoria heap.
		/// 
		/// @pre @p data debe haber sido creado con el operador new.
		/// 
		/// @post Esta instancia de UniquePtr se convierte en due�a del
		/// objeto apuntado por @p data.
		UniquePtr(T* data) : pointer(data) {

		}

		/// @brief Crea un UniquePtr a partir del objeto almacenado en la
		/// direcci�n de memoria dada.
		/// @param data Puntero al objeto almacenado en memoria heap.
		/// 
		/// @note Si este UniquePtr ya ten�a un objeto, dicho objeto se destruir� y se liberar�
		/// su memoria antes de obtener la propiedad de @p data.
		/// 
		/// @pre @p data debe haber sido creado con el operador new.
		/// 
		/// @post Esta instancia de UniquePtr se convierte en due�a del
		/// objeto apuntado por @p data.
		void operator=(T* data) {
			if (pointer)
				delete pointer;
			
			pointer = data;
		}

		/// @brief Destruye el objeto, elimin�ndolo.
		/// Si no ten�a ning�n objeto, no ocurre nada.
		~UniquePtr() {
			if (pointer) {
				delete pointer;
				pointer = nullptr;
			}
		}

		inline UniquePtr(const UniquePtr&) = delete;
		inline UniquePtr& operator=(const UniquePtr&) = delete;


		/// @brief Crea un UniquePtr que pasa a poseer el objeto de otro UniquePtr.
		/// El otr UniquePtr se convierte en un UniquePtr vac�o.
		/// @param other El otro puntero.
		/// 
		/// @post Este UniquePtr pasa a poseer el objeto de @p other.
		/// @post El UniquePtr @p other pasa a estar vac�o.
		/// 
		/// @note Si @p other est� vac�o, entonces este UniquePtr pasa a estar vac�o.
		/// 
		/// @see Swap().
		UniquePtr(UniquePtr&& other) noexcept {
			other.Swap(*this);
		}

		/// <summary>
		/// Este puntero ser� due�o del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		/// <returns>Self.</returns>
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// 
		/// </summary>
		T* Release() {
			T* output = pointer;
			pointer = nullptr;

			return output;
		}

		/// <summary>
		/// Cambia el puntero de este UniquePtr (eliminando el anterior).
		/// </summary>
		/// <param name="newValue">Nuevo valor.</param>
		void SetNewValue(T* newValue) {
			if (pointer)
				delete pointer;

			pointer = newValue;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* GetPointer() const {
			return pointer;
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& GetValue() {
			return *pointer;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* operator->() const {
			return GetPointer();
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& operator*() {
			return GetValue();
		}

		/// <summary>
		/// Devuelve true si el puntero no es null.
		/// </summary>
		/// <returns>Estado del puntero.</returns>
		bool HasValue() const {
			return pointer != nullptr;
		}

		/// <summary>
		/// Elimina el puntero.
		/// </summary>
		inline void Delete() {
			if (pointer)
				delete pointer;

			pointer = nullptr;
		}

		
		/// @brief Intercambia el objeto de este UniquePtr por el
		/// objeto del UniquePtr @p other.
		/// @param other Otro UniquePtr, con el que se va a intercambiar
		/// el objeto.
		/// 
		/// @post Este UniquePtr almacena (y es due�o) del objeto que antes ten�a @p other.
		/// @post El UniquePtr @p other almacena (y es due�o) del objeto que antes ten�a este UniquePtr.
		/// 
		/// @note Si @p other no tiene objeto, este UniquePtr se convierte en un UniquePtr vac�o.
		/// @note Si este UniquePtr no tiene objeto, @p other se convierte en un UniquePtr vac�o.
		void Swap(UniquePtr<T>& other) {
			T* temp = pointer;
			pointer = other.pointer;
			other.pointer = temp;
		}

		bool operator==(const UniquePtr& other) const {
			return pointer == other.pointer;
		}
		bool operator!=(const UniquePtr& other) const {
			return pointer != other.pointer;
		}

	private:

		/// <summary>
		/// Puntero.
		/// </summary>
		T* pointer = nullptr;

	};

	/// <summary>
	/// Es due�o de un puntero.
	/// El puntero original es eliminado al destruirse el UniquePtr.
	/// </summary>
	/// <typeparam name="T">Tipo del puntero.</typeparam>
	template <typename T> class UniquePtr<T[]> {

#define OSK_STD_UP_DELETE(ptr) if (ptr) delete[](ptr);

	public:

		/// <summary>
		/// Crea un UniquePtr vac�o.
		/// </summary>
		UniquePtr() : pointer(nullptr) {

		}

		explicit UniquePtr(T* data) : pointer(data) {

		}

		/// <summary>
		/// Crea un UniquePtr que ser� due�o de 'data'.
		/// </summary>
		/// <param name="data">Puntero original.</param>
		void operator=(T* data) {
			OSK_STD_UP_DELETE(pointer)
				pointer = data;
		}

		/// <summary>
		/// Devuelve el elemento 'i' del array.
		/// No comprueba que est� dentro de los l�mites.
		/// </summary>
		T& operator[](TSize i) const {
			return pointer[i];
		}

		/// <summary>
		/// Destruye el puntero, elimin�ndolo.
		/// <summary/>
		~UniquePtr() {
			OSK_STD_UP_DELETE(pointer)
		}

		inline UniquePtr(const UniquePtr&) = delete;
		inline UniquePtr& operator=(const UniquePtr&) = delete;

		/// <summary>
		/// Este puntero ser� due�o del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		UniquePtr(UniquePtr&& other) {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// Este puntero ser� due�o del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		/// <returns>Self.</returns>
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// 
		/// </summary>
		T* Release() {
			T* output = pointer;
			pointer = nullptr;

			return output;
		}

		/// <summary>
		/// Cambia el puntero de este UniquePtr (eliminando el anterior).
		/// </summary>
		/// <param name="newValue">Nuevo valor.</param>
		void SetNewValue(T* newValue) {
			OSK_STD_UP_DELETE(pointer)

				pointer = newValue;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* GetPointer() const {
			return pointer;
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& GetValue() {
			return *pointer;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* operator->() const {
			return GetPointer();
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& operator*() {
			return GetValue();
		}

		/// <summary>
		/// Devuelve true si el puntero no es null.
		/// </summary>
		/// <returns>Estado del puntero.</returns>
		bool HasValue() const {
			return pointer != nullptr;
		}

		/// <summary>
		/// Elimina el puntero.
		/// </summary>
		inline void Delete() {
			OSK_STD_UP_DELETE(pointer);
			pointer = nullptr;
		}

		void Swap(UniquePtr<T>& other) {
			T* temp = pointer;
			pointer = other.pointer;
			other.pointer = temp;
		}

	private:

		/// <summary>
		/// Puntero.
		/// </summary>
		T* pointer = nullptr;

	};
}

#undef OSK_STD_UP_DELETE
#endif
