#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un car�cter de una fuente.
	OSK_INFO_READ_ONLY
	OSK_INFO_DO_NOT_TOUCH
	struct OSKAPI_CALL OldFontChar	{

		//ID del car�cter.
		OSK_INFO_DO_NOT_TOUCH
		uint32_t ID;

		//Tama�o del car�cter.
		glm::ivec2 Size;

		//Bearing del car�cter.
		glm::ivec2 Bearing;

		//Advance del car�cter.
		uint32_t Advance;

	};

}
