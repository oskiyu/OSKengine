#ifndef OSK_SHARED_PTR
#define OSK_SHARED_PTR

/*
Contiene un puntero al que pueden hacer referencia varios SharedPtr.
Si el último SharedPtr que hace referencia a un puntero es desstruido, el puntero será eliminado.
*/
template <typename T> class SharedPtr {

public:

	/*
	Crea un SharedPtr vacío.
	*/
	SharedPtr() {
		instanceCount = new size_t;
		*instanceCount = 0;
	}

	/*
	Crea un SharedPtr con el puntero dado.

	El puntero debe haberse creado con new.
	*/
	SharedPtr(T* data) {
		pointer = data;

		instanceCount = new size_t;
		*instanceCount = 1;
	}

	/*
	Destruye el SharedPtr.

	Si es el último SharedPtr con este puntero, se eliminará el puntero.
	*/
	~SharedPtr() {
		destructor();
	}

	/*
	Crea un SharedPtr que compartirá su puntero con 'other'.
	*/
	SharedPtr(const SharedPtr& other) {
		share(other);
	}

	/*
	Este SharedPtr compartirá su puntero con 'other'.
	*/
	SharedPtr& operator=(const SharedPtr& other) {
		destructor();

		share(other);

		return *this;
	}


	/*
	El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	*/
	SharedPtr(SharedPtr&& other) {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

	}

	/*
	El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	*/
	SharedPtr& operator=(SharedPtr&& other) noexcept {
		destructor();

		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

		return *this;
	}

	/*
	Devuelve el número de SharedPtr que comparten el mismo puntero que este.
	*/
	size_t GetInstanceCount() const {
		return *instanceCount;
	}

	/*
	Devuelve el puntero.
	*/
	T* GetPointer() const {
		return pointer;
	}

	/*
	Devuelve el puntero.
	*/
	T* operator->() const {
		return GetPointer();
	}

	/*
	Devuelve el valor.
	*/
	T& operator*() const {
		return Get();
	}

	/*
	Devuelve el valor.
	*/
	T& Get() const {
		return *pointer;
	}

	/*
	Devuelve true si el puntero no es null.
	*/
	bool HasValue() const {
		return pointer != nullptr;
	}

private:

	/*
	Destruye el SharedPtr.
	*/
	void destructor() {
		if (!instanceCount)
			return;

		(*instanceCount)--;

		if (*instanceCount == 0)
			finalDelete();

		instanceCount = nullptr;
		pointer = nullptr;
	}

	/*
	Comparte el puntero de 'original'.
	*/
	void share(const SharedPtr& original) {
		pointer = original.pointer;
		instanceCount = original.instanceCount;

		if (pointer != nullptr)
			(*instanceCount)++;
	}

	/*
	El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	*/
	void move(SharedPtr&& other) {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;
	}

	/*
	Elimina los punteros.
	*/
	void finalDelete() {
		if (pointer)
			delete pointer;

		delete instanceCount;
	}

	/*
	Puntero almacenado.
	*/
	T* pointer = nullptr;

	/*
	Número de SharedPtr con este puntero.
	*/
	size_t* instanceCount = nullptr;

};

/*
Contiene un array al que pueden hacer referencia varios SharedArrayPtr.
Si el último SharedArrayPtr que hace referencia a un puntero es desstruido, el puntero será eliminado.
*/
template <typename T> class SharedArrayPtr {

public:

	/*
	Crea un SharedArrayPtr vacío.
	*/
	SharedArrayPtr() {
		instanceCount = new size_t;
		*instanceCount = 0;
	}

	/*
	Crea un SharedArrayPtr con el array dado.

	El array debe haberse creado con new.
	*/
	SharedArrayPtr(T* data) {
		pointer = data;

		instanceCount = new size_t;
		*instanceCount = 1;
	}

	/*
	Destruye el SharedArrayPtr.

	Si es el último SharedArrayPtr con este puntero, se eliminará el puntero.
	*/
	~SharedArrayPtr() {
		destructor();
	}

	/*
	Crea un SharedArrayPtr que compartirá su puntero con 'other'.
	*/
	SharedArrayPtr(const SharedArrayPtr& other) {
		share(other);
	}

	/*
	Este SharedArrayPtr compartirá su puntero con 'other'.
	*/
	SharedArrayPtr& operator=(const SharedArrayPtr& other) {
		destructor();

		share(other);

		return *this;
	}


	/*
	El array de 'other' pasa a ser propiedad de este SharedArrayPtr.
	*/
	SharedArrayPtr(SharedArrayPtr&& other) {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

	}

	/*
	El array de 'other' pasa a ser propiedad de este SharedArrayPtr.
	*/
	SharedArrayPtr& operator=(SharedArrayPtr&& other) noexcept {
		destructor();

		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

		return *this;
	}

	/*
	Devuelve el número de SharedArrayPtr que comparten el mismo puntero que este.
	*/
	size_t GetInstanceCount() const {
		return *instanceCount;
	}

	/*
	Devuelve el array.
	*/
	T* GetArray() const {
		return pointer;
	}

	/*
	Devuelve el valor en la posición 'i'.
	*/
	T& At(size_t i) const {
		return pointer[i];
	}

	/*
	Devuelve el valor en la posición 'i'.
	*/
	T& operator[](size_t i) const {
		return At(i);
	}

private:

	/*
	Destruye el SharedArrayPtr.
	*/
	void destructor() {
		if (!instanceCount)
			return;

		(*instanceCount)--;

		if (*instanceCount == 0)
			Delete();
	}

	/*
	Comparte el puntero de 'original'.
	*/
	void share(const SharedArrayPtr& original) {
		pointer = original.pointer;
		instanceCount = original.instanceCount;

		if (pointer != nullptr)
			(*instanceCount)++;
	}

	/*
	El array de 'other' pasa a ser propiedad de este SharedArrayPtr.
	*/
	void move(SharedArrayPtr&& other) {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;
	}

	/*
	Elimina los punteros.
	*/
	void Delete() {
		if (pointer)
			delete[] pointer;

		delete instanceCount;
	}

	/*
	Puntero almacenado.
	*/
	T* pointer = nullptr;

	/*
	Número de SharedArrayPtr con este puntero.
	*/
	size_t* instanceCount = nullptr;

};

#endif