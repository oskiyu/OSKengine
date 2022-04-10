#pragma once

#include "Color.hpp"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	struct PushConst2D {

		alignas(16) Vector4f texCoords;
		alignas(16) Color color;

	};

}
