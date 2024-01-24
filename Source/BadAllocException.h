#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepción que se lanza cuando no se ha podido reservar la cantidad de memoria indicada.
	class BadAllocException : public EngineException {

	public:

		explicit BadAllocException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("BadAllocException: No se pudo reservar memoria."),
				location) { }

	};

}
