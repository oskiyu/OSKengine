#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void LocalPerMeshData::_SetMaterialInstance(UniquePtr<MaterialInstance>&& instance) {
	m_materialInstance = std::move(instance);
}

void LocalPerMeshData::_SetMaterialBuffer(UniquePtr<GpuBuffer>&& materialBuffer) {
	m_materialBuffer = std::move(materialBuffer);
}

MaterialInstance* LocalPerMeshData::GetMaterialInstance() {
	return m_materialInstance.GetPointer();
}

const MaterialInstance* LocalPerMeshData::GetMaterialInstance() const {
	return m_materialInstance.GetPointer();
}

GpuBuffer* LocalPerMeshData::GetMaterialBuffer() {
	return m_materialBuffer.GetPointer();
}

const GpuBuffer* LocalPerMeshData::GetMaterialBuffer() const {
	return m_materialBuffer.GetPointer();
}
