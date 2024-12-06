#pragma once

#include "EngineException.h"

namespace OSK {

	/// @brief Excepci�n para casos en los que se alcanza c�digo
	/// te�ricamente inaccesible.
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
