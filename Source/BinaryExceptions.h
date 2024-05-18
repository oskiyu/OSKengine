#pragma once

#include "EngineException.h"

namespace OSK {

	class FinishedBlockReaderException : public EngineException {

	public:

		explicit FinishedBlockReaderException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("FinishedBlockReaderException: el bloque ya no contiene más datos."),
				location) { }

	};

	class InvalidBinaryDeserializationException : public EngineException {

	public:

		explicit InvalidBinaryDeserializationException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidBinaryDeserializationException: {}", msg),
				location) { }

	};

}
