#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepci�n que se lanza cuando se trata de cargar un archivo de descripci�n
	/// que no contiene todos los campos necesarios.
	/// Esta excepci�n contendr� detalladamente cu�l es el campo que falta.
	class InvalidDescriptionFileException : public EngineException {

	public:

		explicit InvalidDescriptionFileException(
			std::string_view what,
			std::string_view fileName,
			const std::source_location& location = std::source_location::current())
			: 
			EngineException(
				std::format("InvalidDescriptionFileException: El archivo de recurso {} es inv�lido: {}.", fileName, what),
				location) { }

	};

}
