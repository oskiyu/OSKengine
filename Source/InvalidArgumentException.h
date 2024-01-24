#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepci�n que se lanza cuando se trata de ejecutar una funci�n pasando argumentos
	/// con valores que no cumplen alguna precondici�n.
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
