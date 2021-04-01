#ifndef OSK_UNIQUE_PTR
#define OSK_UNIQUE_PTR

/*
Es dueño de un puntero.
El puntero es eliminado al destruirse el UniquePtr.
*/
template <typename T> class UniquePtr {

public:

	/*
	Crea un UniquePtr vacío.
	*/
	UniquePtr() {

	}

	/*
	Crea un UniquePtr que será dueño de 'data'.
	*/
	UniquePtr(T* data) {
		pointer = data;
	}

	/*
	Elimina el puntero.
	*/
	~UniquePtr() {
		Free();
	}

	UniquePtr(const UniquePtr& other) = delete;
	UniquePtr& operator=(const UniquePtr& other) = delete;

	UniquePtr(UniquePtr&& other) {
		GetOwnership(other);
	}
	UniquePtr& operator=(UniquePtr&& other) {
		GetOwnership(other);

		return *this;
	}

	/*
	Este UniquePtr se convierte en el dueño del puntero de 'other' (eliminando el anterior).
	*/
	void GetOwnership(UniquePtr& other) {
		Free();

		pointer = other.pointer;
		other.pointer = nullptr;
	}

	/*
	Cambia el puntero de este UniquePtr (eliminando el anterior).
	*/
	void SetNewValue(T* newValue) {
		Free();
		pointer = newValue;
	}

	/*
	Devuelve el puntero.
	*/
	T* GetPointer() const {
		return pointer;
	}

	/*
	Devuelve el valor apuntado por el puntero.
	*/
	T& GetValue() {
		return *pointer;
	}

	/*
	Devuelve el puntero.
	*/
	T* operator->() const {
		return GetPointer();
	}

	/*
	Devuelve el valor apuntado por el puntero.
	*/
	T& operator*() {
		return GetValue();
	}

	/*
	Devuelve true si el puntero no es nullptr.
	*/
	bool HasValue() const {
		return pointer != nullptr;
	}

	void Delete() {
		Free();
	}

private:

	/*
	Elimina el puntero.
	*/
	void Free() {
		if (pointer) {
			delete pointer;
			pointer = nullptr;
		}
	}

	/*
	El puntero.
	*/
	T* pointer = nullptr;

};

#endif