#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <limits>

namespace OSK {

	//Compara dos floats.
	//	<a>: float 1.
	//	<b>: float 2.
	//	<epsilon>: límite de la comparación.
	inline bool OSKAPI_CALL CompareFloats(float a, float b, float epsilon = std::numeric_limits<float>::epsilon() * 2) {
		return std::abs(a - b) < epsilon;
	}

}
