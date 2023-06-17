#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepción que se lanza cuando no se consigue encontrar el archivo de descripción
	/// `.json` del asset. 
	/// Asegúrese de que la ruta al archivo es la correcta.
	class AssetDescriptionFileNotFoundException : public EngineException {

	public:

		explicit AssetDescriptionFileNotFoundException(
			std::string_view filePath,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("AssetDescriptionFileNotFoundException: El archivo de descripción {} no se encuentra.", filePath),
				location) { }

	};

}
