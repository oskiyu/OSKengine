#pragma once

#include "EngineException.h"

namespace OSK::IO {

	/// @brief Excepción que se lanza cuando se trata de leer un archivo que no se encuentra.
	class FileNotFoundException : public EngineException {

	public:

		explicit FileNotFoundException(
			std::string_view filePath,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("FileNotFoundException: El archivo {} no existe.", filePath),
				location) { }

	};

}
