#pragma once

#include <mutex>


namespace OSK {

	/// @brief Estructura que contiene un mutex,
	/// de tal manera que permite usarlo como atributo de una
	/// clase sin invalidar las operaciones de copia
	/// y movimiento.
	struct MutexHolder {

		std::mutex mutex{};

		MutexHolder() = default;
		~MutexHolder() = default;

		MutexHolder(const MutexHolder&) {};
		MutexHolder& operator=(const MutexHolder&) { return *this; };

	};

}
