#pragma once

#include "EngineException.h"

namespace OSK::GRAPHICS {

	/// @brief Excepción que se lanza cuando se trata de establecer una matriz a un BLAS que no tiene
	/// creado su buffer de GPU.
	class MatrixBufferNotCreatedException : public EngineException {

	public:

		explicit MatrixBufferNotCreatedException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("MatrixBufferNotCreatedException: El BLAS no tiene inicializado el GPU buffer de matríz."),
				location) { }

	};

	/// @brief Excepción que se lanza cuando hay un error con el API de bajo nivel al crear un AS.
	class AccelerationStructureCreationException : public EngineException {

	public:

		explicit AccelerationStructureCreationException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("AccelerationStructureCreationException: Error al crear el AS. Código: {}.", code),
				location) { }

	};

	/// @brief Excepción que se lanza cuando hay un error con el API de bajo nivel al crear un binding table.
	class RtShaderBindingTableCreationException : public EngineException {

	public:

		explicit RtShaderBindingTableCreationException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("RtShaderBindingTableCreationException: Error al crear la tabla. Código: {}.", code),
				location) { }

	};

}
