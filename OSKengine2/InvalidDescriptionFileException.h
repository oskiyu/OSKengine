#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepción que se lanza cuando se trata de cargar un archivo de descripción
	/// que no contiene todos los campos necesarios.
	/// Esta excepción contendrá detalladamente cuál es el campo que falta.
	class InvalidDescriptionFileException : public EngineException {

	public:

		explicit InvalidDescriptionFileException(
			std::string_view what,
			std::string_view fileName,
			const std::source_location& location = std::source_location::current())
			: 
			EngineException(
				std::format("InvalidDescriptionFileException: El archivo de recurso {} es inválido: {}.", fileName, what),
				location) { }

	};

}
