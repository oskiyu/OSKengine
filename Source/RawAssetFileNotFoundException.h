#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepci�n que se lanza cuando se trata de cargar un archivo de asset raw
	/// que no existe.
	/// Aseg�rese de que la ruta es la correcta.
	class RawAssetFileNotFoundException : public EngineException {

	public:

		explicit RawAssetFileNotFoundException(
			std::string_view filePath,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("RawAssetFileNotFoundException: El archivo de asset {} no se encuentra.", filePath),
				location) { }

	};

}
