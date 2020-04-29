#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Modo del ratón.
	//NORMAL: normal.
	//ALWAYS_RETURN: el ratón siempre se mantiene en el centro de la pantalla y es invisible.
	enum class MouseInputMode {
		NORMAL,
		ALWAYS_RETURN
	};

}
