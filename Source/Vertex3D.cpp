#include "Vertex3D.h"

using namespace OSK::GRAPHICS;

VertexInfo Vertex3D::GetVertexInfo() {
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

	info.entries.Insert({
		sizeof(Vertex3D::tangent),
		VertexInfo::Entry::Type::FLOAT,
		"TANGENT" });

	return info;
}


VertexInfo VertexAnim3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(VertexAnim3D::position),
		VertexInfo::Entry::Type::FLOAT,
		"POSITION" });

	info.entries.Insert({
		sizeof(VertexAnim3D::normal),
		VertexInfo::Entry::Type::FLOAT,
		"NORMAL" });

	info.entries.Insert({
		sizeof(VertexAnim3D::color),
		VertexInfo::Entry::Type::FLOAT,
		"COLOR" });

	info.entries.Insert({
		sizeof(VertexAnim3D::texCoords),
		VertexInfo::Entry::Type::FLOAT,
		"TEXCOORD" });

	info.entries.Insert({
		sizeof(VertexAnim3D::tangent),
		VertexInfo::Entry::Type::FLOAT,
		"TANGENT" });

	info.entries.Insert({
		sizeof(VertexAnim3D::boneIndices),
		VertexInfo::Entry::Type::FLOAT,
		"BONEINDICES" });

	info.entries.Insert({
		sizeof(VertexAnim3D::boneWeights),
		VertexInfo::Entry::Type::FLOAT,
		"BONEWEIGHTS" });

	return info;
}

VertexInfo VertexCollisionDebug3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(VertexCollisionDebug3D::position),
		VertexInfo::Entry::Type::FLOAT,
		"POSITION" });

	return info;
}

VertexInfo GdrVertex3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(GdrVertex3D::gdrIndex),
		VertexInfo::Entry::Type::UNSIGNED_INT,
		"GDR_INDEX" });

	return info;
}

VertexInfo PositionOnlyVertex3D::GetVertexInfo() {
	VertexInfo info{};

	info.entries.Insert({
		sizeof(PositionOnlyVertex3D::position),
		VertexInfo::Entry::Type::FLOAT,
		"POSITION" });

	return info;
}
