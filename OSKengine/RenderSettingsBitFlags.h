#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Ajustes del renderizado.
	enum class RenderSettings {
		DYNAMIC_RES = 1 << 0,
		ENABLE_BLEND = 1 << 1,
		ENABLE_CULL_FACE = 1 << 2,
		ENABLE_DEPTH_TEST = 1 << 3,
		ENABLE_STENCIL_TEST = 1 << 4,
		V_SYNC = 1 << 5
	};

}