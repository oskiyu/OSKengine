#ifndef OSK_UNIQUE_PTR
#define OSK_UNIQUE_PTR

#include "OSKmacros.h"

/// <summary>
/// Es dueño de un puntero.
/// El puntero original es eliminado al destruirse el UniquePtr.
/// </summary>
/// <typeparam name="T">Tipo del puntero.</typeparam>
template <typename T> class UniquePtr {

public:

	/// <summary>
	/// Crea un UniquePtr vacío.
	/// </summary>
	UniquePtr() {

	}

	/// <summary>
	/// Crea un UniquePtr que será dueño de 'data'.
	/// </summary>
	/// <param name="data">Puntero original.</param>
	UniquePtr(T* data) {
		pointer = data;
	}

	/// <summary>
	/// Destruye el puntero, eliminándolo.
	/// <summary/>
	~UniquePtr() {
		Free();
	}

	/// <summary>
	/// Copiar = mover.
	/// </summary>
	inline UniquePtr(const UniquePtr& other) = delete;

	/// <summary>
	/// Copiar = mover.
	/// </summary>
	inline UniquePtr& operator=(const UniquePtr& other) = delete;

	/// <summary>
	/// Este puntero será dueño del puntero de other.
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	UniquePtr(UniquePtr&& other) {
		GetOwnership(other);
	}

	/// <summary>
	/// Este puntero será dueño del puntero de other.
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	/// <returns>Self.</returns>
	UniquePtr& operator=(UniquePtr&& other) noexcept {
		GetOwnership(other);

		return *this;
	}

	/// <summary>
	/// Este UniquePtr se convierte en el dueño del puntero de 'other' (eliminando el anterior).
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	UniquePtr& GetOwnership(UniquePtr& other) {
		Free();

		pointer = other.pointer;
		other.pointer = nullptr;

		return *this;
	}

	/// <summary>
	/// Cambia el puntero de este UniquePtr (eliminando el anterior).
	/// </summary>
	/// <param name="newValue">Nuevo valor.</param>
	void SetNewValue(T* newValue) {
		Free();
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
	void Delete() {
		Free();
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

#endif
