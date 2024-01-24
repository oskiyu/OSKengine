#pragma once

#include "EngineException.h"

namespace OSK::GRAPHICS {

	/// @brief Excepción que se lanza cuando no se pudo resetear la lista de comandos.
	class CommandListResetException : public EngineException {

	public:

		explicit CommandListResetException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("CommandListResetException: código {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se pudo inicializar la lista de comandos.
	class CommandListStartException : public EngineException {

	public:

		explicit CommandListStartException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("CommandListStartException: No se pudo iniciar la lista de comandos. código {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se pudo finalizar el grabado de la lista de comandos.
	class CommandListEndException : public EngineException {

	public:

		explicit CommandListEndException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("CommandListStartException: No se pudo finalizar la lista de comandos. código {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se pudo crear la lista de comandos.
	class CommandListCreationException : public EngineException {

	public:

		explicit CommandListCreationException(
			const size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("CommandListCreationException: No se pudo crear la lista de comandos. código {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando se intenta usar un buffer de vértices que no tiene configurado
	/// su vertex view.
	class InvalidVertexBufferException : public EngineException {

	public:

		explicit InvalidVertexBufferException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidVertexBufferException: El buffer no tiene vertex view."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando se intenta usar un buffer de índices que no tiene configurado
	/// su index view.
	class InvalidIndexBufferException : public EngineException {

	public:

		explicit InvalidIndexBufferException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidIndexBufferException: El buffer no tiene index view."),
				location) { }

	};

}
