#pragma once

#include "Color.hpp"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	struct PushConst2D {

		alignas(16) glm::mat4 matrix;
		alignas(16) Color color;
		alignas(16) Vector4f texCoords;

	};

}
