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
		/// No est� siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Est� siendo pulsado.
		/// </summary>
		PRESSED

	};

}
