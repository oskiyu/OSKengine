#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Botones del ratón.
	enum class OSKAPI_CALL GraphicsAPI {
		OPENGL,
		VULKAN
	};

	std::string OSKAPI_CALL ToString(const GraphicsAPI& graphicsAPI);

}