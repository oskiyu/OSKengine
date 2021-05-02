#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Estados en el que puede estar un bot�n.
	/// </summary>
	enum class OSKAPI_CALL ButtonState {

		/// <summary>
		/// No est� siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Est� siendo pulsado.
		/// </summary>
		PRESSED

	};

}
