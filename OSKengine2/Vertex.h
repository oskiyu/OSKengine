#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

namespace OSK {

	using TIndexSize = uint32_t;

	struct Vertex3D {
		Vector3f position;
		Color color;
		Vector4f texCoords;
	};

}