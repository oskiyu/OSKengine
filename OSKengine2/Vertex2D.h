#pragma once

#include "Vector2.hpp"
#include "Vertex.h"
#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	struct Vertex2D {
		OSK_VERTEX_TYPE_REG("OSK::Vertex2D");

		Vector2f position;
		Vector2f baseTexCoords;

	};

}
