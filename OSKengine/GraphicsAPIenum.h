#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Botones del rat�n.
	enum class GraphicsAPI {
		OPENGL,
		VULKAN
	};

	std::string ToString(const GraphicsAPI& graphicsAPI);

}