#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un carácter de una fuente.
	OSK_INFO_READ_ONLY
	OSK_INFO_DO_NOT_TOUCH
	struct OSKAPI_CALL OldFontChar	{

		//ID del carácter.
		OSK_INFO_DO_NOT_TOUCH
		uint32_t ID;

		//Tamaño del carácter.
		glm::ivec2 Size;

		//Bearing del carácter.
		glm::ivec2 Bearing;

		//Advance del carácter.
		uint32_t Advance;

	};

}
