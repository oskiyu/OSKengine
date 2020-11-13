#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Modos del movimiento del rat�n.
	//ENABLE_PRECISSION: sin 'raw mouse option', con 'precisi�n del puntero'.
	//RAW: sin 'precisi�n del puntero'.
	enum class OSKAPI_CALL MouseMovementMode {
		ENABLE_PRECISSION,
		RAW
	};

}
