#pragma once

#include "EngineException.h"

namespace OSK {

	class ShaderReflectionException : public EngineException {

	public:

		explicit ShaderReflectionException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ShaderReflectionException: {}", msg),
				location) { }

	};

}
