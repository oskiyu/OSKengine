#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"

namespace OSK {

	//Gravedad de los mensajes de Log().
	enum class LogMessageLevels {
		INFO,
		WARNING,
		BAD_ERROR,
		CRITICAL_ERROR,
		COMMAND,
		DEBUG
	};

}

