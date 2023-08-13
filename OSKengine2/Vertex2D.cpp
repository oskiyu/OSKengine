#include "Vertex2D.h"

using namespace OSK::GRAPHICS;

VertexInfo Vertex2D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(Vertex2D::position),
		VertexInfo::Entry::Type::FLOAT,
		"position" });

	info.entries.Insert({
		sizeof(Vertex2D::baseTexCoords),
		VertexInfo::Entry::Type::FLOAT,
		"baseTexCoords" });

	return info;
}
