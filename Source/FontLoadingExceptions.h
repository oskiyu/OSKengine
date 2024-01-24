#pragma once

#include "EngineException.h"

namespace OSK::ASSETS {

	/// @brief Excepción que se lanza cuando ocurre un error al cargar la librería
	/// usada para la generación de fuentes.
	class FontLibraryInitializationException : public EngineException {

	public:

		explicit FontLibraryInitializationException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("FontLibraryInitializationException: No se ha podido cargar FreeType. Code: {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando ocurre un error al cargar una fuente.
	class FontLodaingException : public EngineException {

	public:

		explicit FontLodaingException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("FontLodaingException: No se ha podido cargar la fuente. Code: {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando ocurre un error al cargar un carácter una fuente.
	class FontCharacterLodaingException : public EngineException {

	public:

		explicit FontCharacterLodaingException(
			char character,
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("FontCharacterLodaingException: No se ha podido cargar el carácter {}. Code: {}.", character, code),
				location) { }

	};

}
