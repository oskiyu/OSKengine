#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Modos de aceleraci�n por software que puede tener el rat�n.
	/// </summary>
	enum class OSKAPI_CALL MouseAccelerationMode {

		/// <summary>
		/// Aceleraci�n activada.
		/// </summary>
		ENABLE_PRECISSION,

		/// <summary>
		/// Aceleraci�n desactivada.
		/// </summary>
		RAW

	};

}
