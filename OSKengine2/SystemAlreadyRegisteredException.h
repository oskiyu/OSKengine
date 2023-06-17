#pragma once

#include "EngineException.h"

namespace OSK::ECS {

	class SystemAlreadyRegisteredException : public EngineException {
	
	public:

		explicit SystemAlreadyRegisteredException(
			std::string_view systemName,
			const std::source_location& location = std::source_location::current())
			 : 
			EngineException(
				std::format("El sistema {} ya está registrado.", systemName),
				location) {}

	};

}
