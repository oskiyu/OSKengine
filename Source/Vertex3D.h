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
		Vector3f tangent;

	};

	struct VertexAnim3D {
		OSK_VERTEX_TYPE_REG("OSK::VertexAnim3D");

		Vector3f position;
		Vector3f normal;
		Color color;
		Vector2f texCoords;
		Vector3f tangent;

		Vector4f boneIndices;
		Vector4f boneWeights;
	};

	struct VertexCollisionDebug3D {
		OSK_VERTEX_TYPE_REG("OSK::VertexCollisionDebug3D");

		Vector3f position;
	};

	struct GdrVertex3D {
		OSK_VERTEX_TYPE_REG("OSK::GdrVertex3D");
		USize32 gdrIndex = 0;
	};

	struct PositionOnlyVertex3D {
		OSK_VERTEX_TYPE_REG("OSK::PositionOnlyVertex3D");
		Vector3f position = Vector3f::Zero;
	};

}
