#include "Vertex3D.h"

using namespace OSK::GRAPHICS;

const VertexInfo Vertex3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(Vertex3D::position),
		VertexInfo::Entry::Type::FLOAT,
		"POSITION" });

	info.entries.Insert({
		sizeof(Vertex3D::normal),
		VertexInfo::Entry::Type::FLOAT,
		"NORMAL" });

	info.entries.Insert({
		sizeof(Vertex3D::color),
		VertexInfo::Entry::Type::FLOAT,
		"COLOR" });

	info.entries.Insert({
		sizeof(Vertex3D::texCoords),
		VertexInfo::Entry::Type::FLOAT,
		"TEXCOORD" });

	return info;
}
