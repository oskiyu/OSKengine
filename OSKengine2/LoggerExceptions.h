#pragma once

#include "EngineException.h"

namespace OSK::IO {

	/// @brief Excepción que se lanza cuando se trata de usar un logger que no ha sido inicializado correctamente.
	class LoggerNotInitializedException : public EngineException {

	public:

		explicit LoggerNotInitializedException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				"LoggerNotInitializedException: OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.",
				location) { }

	};

}
