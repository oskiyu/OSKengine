#pragma once

#include "EngineException.h"

namespace OSK {

	class OSKAPI_CALL DivideByZeroException : public EngineException {

	public:

		DivideByZeroException(const std::source_location& location = std::source_location::current())
			:
			EngineException("DivideByZeroException: se ha intentado dividir un n�mero por 0.") { }

	};

	class OSKAPI_CALL OverflowConversionException : public EngineException {

	public:

		OverflowConversionException(const std::source_location& location = std::source_location::current())
			:
			EngineException("OverflowConversionException: se ha intentado convertir un n�mero, dando resultado un overflow.") { }

	};

}
