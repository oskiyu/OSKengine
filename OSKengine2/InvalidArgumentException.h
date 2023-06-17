#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepción que se lanza cuando se trata de ejecutar una función pasando argumentos
	/// con valores que no cumplen alguna precondición.
	class InvalidArgumentException : public EngineException {

	public:

		explicit InvalidArgumentException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidArgumentException: {}.", msg),
				location) { }

	};

}
