#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void MeshMapping::RegisterModel(UIndex64 modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel[modelId] = {};
}

void MeshMapping::UnregisterModel(UIndex64 modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel.erase(modelId);
}

PerModelData& MeshMapping::GetModelData(UIndex64 modelId) {
	return m_meshesPerModel.at(modelId);
}

const PerModelData& MeshMapping::GetModelData(UIndex64 modelId) const {
	return m_meshesPerModel.at(modelId);
}

bool MeshMapping::HasModel(UIndex64 modelId) const {
	return m_meshesPerModel.contains(modelId);
}
