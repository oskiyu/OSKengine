#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void LocalPerMeshData::_SetMaterialInstance(OwnedPtr<MaterialInstance> instance) {
	m_materialInstance = instance.GetPointer();
}

void LocalPerMeshData::_SetMaterialBuffer(OwnedPtr<GpuBuffer> materialBuffer) {
	m_materialBuffer = materialBuffer.GetPointer();
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
