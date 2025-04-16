#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


MaterialInstance* GlobalPerModelData::GetAnimationMaterialInstance() {
	return m_animationMaterialInstance.GetPointer();
}

const MaterialInstance* GlobalPerModelData::GetAnimationMaterialInstance() const {
	return m_animationMaterialInstance.GetPointer();
}
const GpuBuffer* GlobalPerModelData::GetBonesBuffer() const {
	return m_bonesBuffer.GetPointer();
}

GpuBuffer* GlobalPerModelData::GetBonesBuffer() {
	return m_bonesBuffer.GetPointer();
}

void GlobalPerModelData::SetAnimationMaterialInstance(UniquePtr<MaterialInstance>&& instance) {
	m_animationMaterialInstance = std::move(instance);
}

void GlobalPerModelData::SetBonesBuffer(UniquePtr<GpuBuffer>&& buffer) {
	m_bonesBuffer = std::move(buffer);
}


// --- Local --- //

void LocalPerModelData::RegisterMesh(GpuMeshUuid meshId) {
	if (HasMesh(meshId))
		return;

	m_meshesPerModel.try_emplace(meshId);
}

void LocalPerModelData::UnregisterMesh(GpuMeshUuid meshId) {
	if (HasMesh(meshId))
		return;

	m_meshesPerModel.erase(meshId);
}

LocalPerMeshData& LocalPerModelData::GetMeshData(GpuMeshUuid meshId) {
	return m_meshesPerModel.at(meshId);
}

const LocalPerMeshData& LocalPerModelData::GetMeshData(GpuMeshUuid meshId) const {
	return m_meshesPerModel.at(meshId);
}

bool LocalPerModelData::HasMesh(GpuMeshUuid meshId) const {
	return m_meshesPerModel.contains(meshId);
}
