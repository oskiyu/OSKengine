#pragma once

#include "EngineException.h"

namespace OSK::ECS {

	/// @brief Excepción que se lanza cuando se trata de usar un evento que no ha sido registrado.
	class EventNotRegisteredException : public EngineException {

	public:

		explicit EventNotRegisteredException(
			std::string_view eventName,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("EventNotRegisteredException: El evento {} no está registrado.", eventName),
				location) { }

	};

	/// @brief Excepción que se lanza cuando se trata de registrar un evento que ya ha sido registrado.
	class EventAlreadyRegisteredException : public EngineException {

	public:

		explicit EventAlreadyRegisteredException(
			std::string_view eventName,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("EventAlreadyRegisteredException: El evento {} ya estaba registrado.", eventName),
				location) { }

	};

}
