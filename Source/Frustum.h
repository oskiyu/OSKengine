#pragma once

#include "Plane.h"
#include "DynamicArray.hpp"

namespace OSK {

	/// @brief Frustum con un número indeterminado de planos.
	using AnyFrustum = DynamicArray<Plane>;

}
