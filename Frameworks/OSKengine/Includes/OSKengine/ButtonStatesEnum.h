#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Estados en el que puede estar un botón.
	/// </summary>
	enum class OSKAPI_CALL ButtonState {

		/// <summary>
		/// No está siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Está siendo pulsado.
		/// </summary>
		PRESSED

	};

}
