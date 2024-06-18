#pragma once

#include "EngineException.h"

namespace OSK {

	class PostProcessInputNotSetException : public EngineException {

	public:

		explicit PostProcessInputNotSetException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("PostProcessInputNotSetException: {}", msg),
				location) { }

	};

}
