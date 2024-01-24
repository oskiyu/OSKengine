#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepción que se lanza cuando se trata de ejecutar una función de un objeto
	/// cuyo estado interno no es el necesario.
	class InvalidObjectStateException : public EngineException {

	public:

		explicit InvalidObjectStateException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidObjectStateException: {}.", msg),
				location) { }

	};

}
