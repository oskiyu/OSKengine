#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

namespace OSK::GRAPHICS {

	using TIndexSize = uint32_t;

	struct Vertex3D {
		Vector3f position;
		float dx12discard = 1.0f;
		Color color;
		Vector2f texCoords;
	};

}
