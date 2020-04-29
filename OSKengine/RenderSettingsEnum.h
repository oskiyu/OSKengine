#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Ajustes del renderizado.
	enum class RenderSettings {
		DYNAMIC_RES,
		ENABLE_BLEND,
		ENABLE_CULL_FACE,
		ENABLE_DEPTH_TEST,
		ENABLE_STENCIL_TEST,
		V_SYNC
	};

}