#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Modo del rat�n.
	//NORMAL: normal.
	//ALWAYS_RETURN: el rat�n siempre se mantiene en el centro de la pantalla y es invisible.
	enum class MouseInputMode {
		NORMAL,
		ALWAYS_RETURN
	};

}
