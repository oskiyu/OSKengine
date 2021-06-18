#ifndef OSK_SHARED_PTR
#define OSK_SHARED_PTR

#include <functional>

#include "OSKmacros.h"

/// <summary>
/// Contiene un puntero al que pueden hacer referencia varios SharedPtr.
/// Si el último SharedPtr que hace referencia a un puntero es desstruido, el puntero será eli
/// </summary>
/// <typeparam name="T">Tipo del puntero almacenado.</typeparam>
template <typename T> class OSKAPI_CALL SharedPtr {

public:

	/// <summary>
	/// Crea un SharedPtr vacío.
	/// </summary>
	SharedPtr() {
		instanceCount = new size_t;
		*instanceCount = 0;
	}

	/// <summary>
	/// Crea un SharedPtr con el puntero dado.
	/// El puntero debe haberse creado con new.
	/// </summary>
	/// <param name="data">Puntero.</param>
	SharedPtr(T* data) {
		pointer = data;

		instanceCount = new size_t;
		*instanceCount = 1;
	}

	/// <summary>
	/// Destruye el SharedPtr.
	/// Si es el último SharedPtr con este puntero, se eliminará el puntero.
	/// <summary/>
	~SharedPtr() {
		destructor();
	}

	/// <summary>
	/// Crea un SharedPtr que compartirá su puntero con 'other'.
	/// </summary>
	/// <param name="other">Puntero compartido.</param>
	SharedPtr(const SharedPtr& other) {
		share(other);
	}

	/// <summary>
	/// Este SharedPtr compartirá su puntero con 'other'.
	/// </summary>
	/// <param name="other">Puntero compartido.</param>
	/// <returns>Self</returns>
	SharedPtr& operator=(const SharedPtr& other) {
		destructor();

		share(other);

		return *this;
	}

	/// <summary>
	/// El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	SharedPtr(SharedPtr&& other) noexcept {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

	}

	/// <summary>
	/// El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	/// <returns>Self.</returns>
	SharedPtr& operator=(SharedPtr&& other) noexcept {
		destructor();

		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;

		return *this;
	}

	/// <summary>
	/// Devuelve el número de SharedPtr que comparten el mismo puntero que este.
	/// </summary>
	/// <returns>Número de instancias.</returns>
	size_t GetInstanceCount() const {
		return *instanceCount;
	}

	/// <summary>
	/// Devuelve el puntero nativo.
	/// </summary>
	/// <returns>Puntero.</returns>
	T* GetPointer() const {
		return pointer;
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
	T& operator*() const {
		return Get();
	}

	/// <summary>
	/// Devuelve el valor apuntado por el puntero.
	/// </summary>
	/// <returns>Valor.</returns>
	T& Get() const {
		return *pointer;
	}

	/// <summary>
	/// Devuelve true si el puntero no es null.
	/// </summary>
	/// <returns>Estado del puntero.</returns>
	bool HasValue() const {
		return pointer != nullptr;
	}

	/// <summary>
	/// Función que se ejecuta al eliminar a la última instancia del puntero compartido.
	/// Por defecto, llama a delete.
	/// </summary>
	std::function<void()> Deleter = [this]() {
		if (pointer)
			delete pointer;
	};

private:

	/// <summary>
	/// Destruye el SharedPtr.
	/// </summary>
	void destructor() {
		if (!instanceCount)
			return;

		(*instanceCount)--;

		if (*instanceCount == 0)
			finalDelete();

		instanceCount = nullptr;
		pointer = nullptr;
	}

	/// <summary>
	/// Comparte el puntero de 'original'.
	/// </summary>
	/// <param name="original">Puntero compartido.</param>
	void share(const SharedPtr& original) {
		pointer = original.pointer;
		instanceCount = original.instanceCount;

		if (pointer != nullptr)
			(*instanceCount)++;
	}

	/// <summary>
	/// El puntero de 'other' pasa a ser propiedad de este SharedPtr.
	/// </summary>
	/// <param name="other">Otro puntero.</param>
	void move(SharedPtr&& other) {
		this->pointer = other.pointer;
		this->instanceCount = other.instanceCount;

		other.pointer = nullptr;
		other.instanceCount = nullptr;
	}

	/// <summary>
	/// Elimina los punteros.
	/// </summary>
	void finalDelete() {
		Deleter();

		delete instanceCount;
	}

	/// <summary>
	/// Puntero compartido.
	/// </summary>
	T* pointer = nullptr;

	/// <summary>
	/// Número de SharedPtr con este puntero.
	/// </summary>
	size_t* instanceCount = nullptr;

};

#endif
