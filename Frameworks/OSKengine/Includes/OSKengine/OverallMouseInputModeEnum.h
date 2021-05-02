#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Comportamiento del rat�n.
	/// </summary>
	enum class OSKAPI_CALL MouseInputMode {

		/// <summary>
		/// El rat�n se mueve libremente.
		/// </summary>
		FREE,

		/// <summary>
		/// El rat�n siempre est� en el centro de la pantalla.
		/// </summary>
		ALWAYS_RETURN

	};

}
