#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <limits>

namespace OSK {

	/// <summary>
	/// Compara dos floats.
	/// </summary>
	/// <param name="a">float 1.</param>
	/// <param name="b">float 2.</param>
	/// <param name="epsilon">Límite de la comparación.</param>
	/// <returns>True si son comparables.</returns>
	inline bool CompareFloats(float a, float b, float epsilon = std::numeric_limits<float>::epsilon() * 2) {
		return std::abs(a - b) < epsilon;
	}

}
