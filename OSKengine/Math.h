#pragma once

#include <limits>

namespace OSK {

	inline bool CompareFloats(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon() * 2) {
		return std::abs(a - b) < epsilon;
	}

}
