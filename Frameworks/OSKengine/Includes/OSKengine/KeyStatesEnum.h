#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Indica el estado de una tecla del teclado.
	// PRESSED
	// RELEASED
	enum class OSKAPI_CALL KeyState {
		RELEASED,
		PRESSED
	};

}
