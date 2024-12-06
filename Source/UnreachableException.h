#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepción para casos en los que se alcanza código
	/// teóricamente inaccesible.
	class UnreachableException : public EngineException {

	public:

		explicit UnreachableException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("UnreachableException: {}", msg),
				location) { }

	};

}
