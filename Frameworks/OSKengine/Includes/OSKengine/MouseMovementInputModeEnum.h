#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Modos del movimiento del ratón.
	//ENABLE_PRECISSION: sin 'raw mouse option', con 'precisión del puntero'.
	//RAW: sin 'precisión del puntero'.
	enum class OSKAPI_CALL MouseMovementMode {
		ENABLE_PRECISSION,
		RAW
	};

}
