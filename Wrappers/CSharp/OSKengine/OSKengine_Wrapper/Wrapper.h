#pragma once

template <typename T> public ref class Wrapper {

public:

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