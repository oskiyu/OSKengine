#pragma once

#include <atomic>


namespace OSK {

	/// @brief Estructura que contiene una variable atómica,
	/// de tal manera que permite usarla como atributo de una
	/// clase sin invalidar las operaciones de copia
	/// y movimiento.
	/// @tparam T Tipo de dato.
	/// 
	/// @pre @p T debe ser usable como argumento de plantilla de std::atomic.
	template <typename T>
	struct AtomicHolder {

		std::atomic<T> atomic{};

		AtomicHolder() = default;
		explicit AtomicHolder(const T& value) : atomic(value) {}
		~AtomicHolder() = default;

		AtomicHolder(const AtomicHolder& other) : atomic(other.atomic.load()) {};
		AtomicHolder& operator=(const AtomicHolder& other) { atomic = other.atomic.load(); return *this; };

	};

}
