#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Comportamiento del ratón.
	/// </summary>
	enum class OSKAPI_CALL MouseInputMode {

		/// <summary>
		/// El ratón se mueve libremente.
		/// </summary>
		FREE,

		/// <summary>
		/// El ratón siempre está en el centro de la pantalla.
		/// </summary>
		ALWAYS_RETURN

	};

}
