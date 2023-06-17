#pragma once

#include "EngineException.h"

namespace OSK {

	class OSKAPI_CALL WindowNotCreatedException : public EngineException {

	public:

		WindowNotCreatedException(const std::source_location& location = std::source_location::current())
			:
			EngineException("WindowNotCreatedException: No se ha creado correctamente la ventana en CreateWindow().") { }

	};

	class OSKAPI_CALL RenderedNotCreatedException : public EngineException {

	public:

		RenderedNotCreatedException(const std::source_location& location = std::source_location::current())
			:
			EngineException("RenderedNotCreatedException: No se ha inicializado correctamente el renderizador en SetupEngine().") { }

	};

}
