#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Representa una versión.
	/// </summary>
	struct OSKAPI_CALL Version {

		/// <summary>
		/// Versión mayor.
		/// </summary>
		unsigned int mayor;

		/// <summary>
		/// Versión menor.
		/// </summary>
		unsigned int menor;

		/// <summary>
		/// Parche.
		/// </summary>
		unsigned int parche;

	};

}
