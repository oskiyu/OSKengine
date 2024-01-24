#pragma once

#include "EngineException.h"

namespace OSK::GRAPHICS {

	/// @brief Excepción que se lanza cuando se trata de usar un evento que no ha sido registrado.
	class PipelineCreationException : public EngineException {

	public:

		explicit PipelineCreationException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("PipelineCreationException: Error al crear el compute pipeline. Código: {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando hay un error con el API de bajo nivel al cargar un shader.
	class ShaderLoadingException : public EngineException {

	public:

		explicit ShaderLoadingException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ShaderLoadingException: Error al cargar el shader. Código: {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando hay un error con el API de bajo nivel al compilar un shader.
	class ShaderCompilingException : public EngineException {

	public:

		explicit ShaderCompilingException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ShaderCompilingException: Error al compilar el shader. Código: {}.", code),
				location) { }

		explicit ShaderCompilingException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ShaderCompilingException: Error al compilar el shader. Mensaje: {}.", msg),
				location) { }

	};

	/// @brief Excepción que se lanza cuando no se pudo crear el pipeline layout nativo.
	class PipelineLayoutCreationException : public EngineException {

	public:

		explicit PipelineLayoutCreationException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("PipelineLayoutCreationException: Error nativo al crear layout. Code: {}.", code),
				location) { }

	};

}
