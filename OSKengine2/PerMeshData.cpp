#include "MeshMapping.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void PerMeshData::_SetMaterialInstance(OwnedPtr<MaterialInstance> instance) {
	m_materialInstance = instance.GetPointer();
}

void PerMeshData::_SetBlas(OwnedPtr<IBottomLevelAccelerationStructure> blas) {
	m_bottomLevelAS = blas.GetPointer();
}

MaterialInstance* PerMeshData::GetMaterialInstance() {
	return m_materialInstance.GetPointer();
}

const MaterialInstance* PerMeshData::GetMaterialInstance() const {
	return m_materialInstance.GetPointer();
}
