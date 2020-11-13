#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Representa una versión.
	struct OSKAPI_CALL Version {
		//Versión mayor.
		unsigned int Mayor;
		//Versión menor.
		unsigned int Menor;
		//Parche.
		unsigned int Parche;
	};

}
