#include "Vertex3D.h"

using namespace OSK::GRAPHICS;

const VertexInfo Vertex3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(Vertex3D::position),
		VertexInfo::Entry::Type::FLOAT,
		"position" });

	info.entries.Insert({
		sizeof(Vertex3D::normal),
		VertexInfo::Entry::Type::FLOAT,
		"normal" });

	info.entries.Insert({
		sizeof(Vertex3D::color),
		VertexInfo::Entry::Type::FLOAT,
		"color" });

	info.entries.Insert({
		sizeof(Vertex3D::texCoords),
		VertexInfo::Entry::Type::FLOAT,
		"texCoord" });

	return info;
}
