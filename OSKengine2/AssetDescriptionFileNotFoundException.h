#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepci�n que se lanza cuando no se consigue encontrar el archivo de descripci�n
	/// `.json` del asset. 
	/// Aseg�rese de que la ruta al archivo es la correcta.
	class AssetDescriptionFileNotFoundException : public EngineException {

	public:

		explicit AssetDescriptionFileNotFoundException(
			std::string_view filePath,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("AssetDescriptionFileNotFoundException: El archivo de descripci�n {} no se encuentra.", filePath),
				location) { }

	};

}
