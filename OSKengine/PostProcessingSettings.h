#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

namespace OSK {

	struct OSKAPI_CALL PostProcessingSettings_t {
		 int UseFXAA = 1;
		 int ScreenSizeX;
		 int ScreenSizeY;
	};

}
