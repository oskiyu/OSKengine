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
	//	<epsilon>: l�mite de la comparaci�n.
	inline bool OSKAPI_CALL CompareFloats(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon() * 2) {
		return std::abs(a - b) < epsilon;
	}

}
