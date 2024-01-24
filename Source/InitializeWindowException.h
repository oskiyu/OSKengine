#pragma once

#include "EngineException.h"

namespace OSK::IO {

	/// @brief Excepción que se lanza cuando no se puede crear la ventana del juego.
	class InitializeWindowException : public EngineException {

	public:

		explicit InitializeWindowException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				"No se ha podido iniciar la ventana GLFW.",
				location) { }

	};

}
