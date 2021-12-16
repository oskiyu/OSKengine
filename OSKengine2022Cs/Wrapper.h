#pragma once

/// <summary>
/// Clase managed que almacena una instancia de una clase nativa.
/// Se encarga de limpiar memoria al ser eliminada.
/// </summary>
template <typename T> public ref class Wrapper {

public:

	Wrapper() {
		instance = new T;
	}

	Wrapper(T* instance) {
		this->instance = instance;
	}

	virtual ~Wrapper() {
		if (instance)
			delete instance;
	}

	!Wrapper() {
		if (instance)
			delete instance;
	}

	void SetNativeValue(T value) {
		*instance = value;
	}

protected:

	T* instance = nullptr;

};
