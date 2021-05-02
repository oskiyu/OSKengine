#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Modos de aceleración por software que puede tener el ratón.
	/// </summary>
	enum class OSKAPI_CALL MouseAccelerationMode {

		/// <summary>
		/// Aceleración activada.
		/// </summary>
		ENABLE_PRECISSION,

		/// <summary>
		/// Aceleración desactivada.
		/// </summary>
		RAW

	};

}
