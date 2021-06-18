#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Representa una versi�n.
	/// </summary>
	struct OSKAPI_CALL Version {

		/// <summary>
		/// Versi�n mayor.
		/// </summary>
		unsigned int mayor;

		/// <summary>
		/// Versi�n menor.
		/// </summary>
		unsigned int menor;

		/// <summary>
		/// Parche.
		/// </summary>
		unsigned int parche;

	};

}
