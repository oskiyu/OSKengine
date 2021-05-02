#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Posibles estados de una tecla del teclado.
	/// </summary>
	enum class OSKAPI_CALL KeyState {

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
