#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepci�n que se lanza cuando se trata de ejecutar una funcionalidad que a�n no se ha desarrollado.
	class NotImplementedException : public EngineException {

	public:

		explicit NotImplementedException(
			const std::source_location& location = std::source_location::current())
			 :
			EngineException(
				std::format("NotImplementedException."),
				location) { }

	};

}
