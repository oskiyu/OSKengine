#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "VertexInfo.h"
#include "Vertex.h"

namespace OSK::GRAPHICS {

	struct Vertex3D {
		OSK_VERTEX_TYPE_REG("OSK::Vertex3D");

		Vector3f position;
		Vector3f normal;
		Color color;
		Vector2f texCoords;

	};

	struct VertexAnim3D {
		OSK_VERTEX_TYPE_REG("OSK::VertexAnim3D");

		Vector3f position;
		Vector3f normal;
		Color color;
		Vector2f texCoords;

		Vector4f boneIndices;
		Vector4f boneWeights;
	};

}
