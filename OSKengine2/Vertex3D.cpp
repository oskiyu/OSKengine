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


const VertexInfo VertexAnim3D::GetVertexInfo() {
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
		sizeof(VertexAnim3D::boneIndices),
		VertexInfo::Entry::Type::FLOAT,
		"BONEINDICES" });

	info.entries.Insert({
		sizeof(VertexAnim3D::boneWeights),
		VertexInfo::Entry::Type::FLOAT,
		"BONEWEIGHTS" });

	return info;
}
