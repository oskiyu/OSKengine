#pragma once

#include "Plane.h"
#include "DynamicArray.hpp"

namespace OSK {

	/// @brief Frustum con un n�mero indeterminado de planos.
	using AnyFrustum = DynamicArray<Plane>;

}
