#pragma once

#include "EngineException.h"

namespace OSK {

	class OSKAPI_CALL DivideByZeroException : public EngineException {

	public:

		DivideByZeroException(const std::source_location& location = std::source_location::current())
			:
			EngineException("DivideByZeroException: se ha intentado dividir un número por 0.") { }

	};

}
