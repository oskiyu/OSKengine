#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void PerModelData::RegisterMesh(UIndex64 meshId) {
	if (HasMesh(meshId))
		return;

	m_meshesPerModel.try_emplace(meshId);
}

void PerModelData::UnregisterMesh(UIndex64 meshId) {
	if (HasMesh(meshId))
		return;

	m_meshesPerModel.erase(meshId);
}

PerMeshData& PerModelData::GetMeshData(UIndex64 meshId) {
	return m_meshesPerModel.at(meshId);
}

const PerMeshData& PerModelData::GetMeshData(UIndex64 meshId) const {
	return m_meshesPerModel.at(meshId);
}

bool PerModelData::HasMesh(UIndex64 meshId) const {
	return m_meshesPerModel.contains(meshId);
}
