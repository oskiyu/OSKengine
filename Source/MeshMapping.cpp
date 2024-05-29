#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void GlobalMeshMapping::RegisterModel(GpuModelUuid modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel.try_emplace(modelId);
}

void GlobalMeshMapping::UnregisterModel(GpuModelUuid modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel.erase(modelId);
}

GlobalPerModelData& GlobalMeshMapping::GetModelData(GpuModelUuid modelId) {
	return m_meshesPerModel.at(modelId);
}

const GlobalPerModelData& GlobalMeshMapping::GetModelData(GpuModelUuid modelId) const {
	return m_meshesPerModel.at(modelId);
}

bool GlobalMeshMapping::HasModel(GpuModelUuid modelId) const {
	return m_meshesPerModel.contains(modelId);
}

void GlobalMeshMapping::SetPreviousModelMatrix(ECS::GameObjectIndex obj, glm::mat4 matrix) {
	m_previousModelMatrices[obj] = matrix;
}

glm::mat4 GlobalMeshMapping::GetPreviousModelMatrix(ECS::GameObjectIndex obj) const {
	OSK_ASSERT(HasPreviousModelMatrix(obj), InvalidArgumentException(std::format("El objeto {} no está registrado.", obj.Get())))
	return m_previousModelMatrices.at(obj);
}

void GlobalMeshMapping::RemovePreviousModelMatrix(ECS::GameObjectIndex obj) {
	OSK_ASSERT(HasPreviousModelMatrix(obj), InvalidArgumentException(std::format("El objeto {} no está registrado.", obj.Get())))
	m_previousModelMatrices.erase(obj);
}

bool GlobalMeshMapping::HasPreviousModelMatrix(ECS::GameObjectIndex obj) const {
	return m_previousModelMatrices.contains(obj);
}


// --- Local --- //

void LocalMeshMapping::RegisterModel(GpuModelUuid modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel.try_emplace(modelId);
}

void LocalMeshMapping::UnregisterModel(GpuModelUuid modelId) {
	if (HasModel(modelId))
		return;

	m_meshesPerModel.erase(modelId);
}

LocalPerModelData& LocalMeshMapping::GetModelData(GpuModelUuid modelId) {
	OSK_ASSERT(HasModel(modelId), InvalidArgumentException(std::format("El modelo {} no está registrado.", modelId.Get())))
	return m_meshesPerModel.at(modelId);
}

const LocalPerModelData& LocalMeshMapping::GetModelData(GpuModelUuid modelId) const {
	OSK_ASSERT(HasModel(modelId), InvalidArgumentException(std::format("El modelo {} no está registrado.", modelId.Get())))
	return m_meshesPerModel.at(modelId);
}

bool LocalMeshMapping::HasModel(GpuModelUuid modelId) const {
	return m_meshesPerModel.contains(modelId);
}
