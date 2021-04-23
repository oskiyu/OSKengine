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
		unsigned int Mayor;

		/// <summary>
		/// Versi�n menor.
		/// </summary>
		unsigned int Menor;

		/// <summary>
		/// Parche.
		/// </summary>
		unsigned int Parche;

	};

}
