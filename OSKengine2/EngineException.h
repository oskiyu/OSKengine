#pragma once

#include <stdexcept>
#include <source_location>
#include <format>

#include "OSKmacros.h"

namespace OSK {

	/// @brief Clase base para todas las excepciones que puedan ocurrir
	/// durante la ejecuci�n del juego.
	class OSKAPI_CALL EngineException : public std::runtime_error {

	public:

		~EngineException() override = default;

		OSK_DEFAULT_COPY_OPERATOR(EngineException);
		OSK_DEFAULT_MOVE_OPERATOR(EngineException);
		
		/// @brief Crea la excepci�n.
		/// @param message Mensaje de la excepci�n. Debe incluir una descripci�n
		/// detallada.
		/// @param location Lugar en el que ocurri� la excepci�n.
		inline EngineException(
			const std::string_view message, 
			const std::source_location& location = std::source_location::current())
			: 
			std::runtime_error(std::format("{}\n{}", message, FormatLocation(location))),
			location(location) { }

	private:

		/// @brief Localizac�n del error (l�nea, etc...).
		std::source_location location;

		static inline std::string FormatLocation(const std::source_location& location) {
			return std::format("\tat {}\n\tat line {}\n\tat file {}",
				location.function_name(),
				location.line(),
				location.file_name());
		}

	};

}
