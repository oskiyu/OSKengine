#ifndef OSK_OWNED_PTR
#define OSK_OWNED_PTR

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Es dueño de un puntero.
	/// 
	/// @note El puntero original NO es eliminado al destruirse el OwnedPtr.
	/// @warning Se desaconseja su uso como miembro de clase.
	/// No copia el recurso si se copia la clase, lo que puede generar problemas.
	/// </summary>
	/// <typeparam name="T">Tipo del puntero.</typeparam>
	template <typename T> class OwnedPtr {

	public:

		/// <summary>
		/// Crea un OwnedPtr vacío.
		/// </summary>
		OwnedPtr() {

		}

		/// <summary>
		/// Crea un OwnedPtr que será dueño de 'data'.
		/// </summary>
		/// <param name="data">Puntero original.</param>
		OwnedPtr(T* data) {
			pointer = data;
		}
		/// <summary>
		/// Crea un SharedPtr que compartirá su puntero con 'other'.
		/// </summary>
		/// <param name="other">Puntero compartido.</param>
		OwnedPtr(const OwnedPtr& other) {
			this->pointer = other.pointer;
		}

		/// <summary>
		/// Copia el OwnedPtr.
		/// </summary>
		OwnedPtr& operator=(const OwnedPtr& other) {
			this->pointer = other.pointer;

			return *this;
		}

		OwnedPtr& operator=(OwnedPtr&& other) {
			std::swap(this->pointer, other.pointer);
			return *this;
		}

		OwnedPtr(OwnedPtr&& other) {
			this->pointer = other.pointer;
			other.pointer = nullptr;
		}

		/// <summary>
		/// Accede a un elemento, considerando el puntero un array.
		/// </summary>
		const T& operator[](size_t i) const {
			return pointer[i];
		}

		/// <summary>
		/// Accede a un elemento, considerando el puntero un array.
		/// </summary>
		T& operator[](size_t i) {
			return pointer[i];
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
		void Delete() {
			Free();
		}

		bool operator==(const OwnedPtr& other) const {
			return pointer == other.pointer;
		}
		bool operator!=(const OwnedPtr& other) const {
			return pointer != other.pointer;
		}

	private:

		/// <summary>
		/// Elimina el puntero.
		/// </summary>
		void Free() {
			if (pointer) {
				delete pointer;
				pointer = nullptr;
			}
		}

		/// <summary>
		/// Puntero.
		/// </summary>
		T* pointer = nullptr;

	};

}

#endif
