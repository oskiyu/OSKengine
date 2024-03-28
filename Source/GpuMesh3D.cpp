#include "GpuMesh3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


GpuMesh3D::GpuMesh3D(GpuMeshUuid uuid, UIndex32 firstIndex, USize32 numIndices, const Vector3f& sphereCenter, UIndex64 materialIndex)
	: m_uuid(uuid), m_firstIndex(firstIndex), m_numIndices(numIndices), m_sphereCenter(sphereCenter), m_materialIndex(materialIndex) {}

UIndex32 GpuMesh3D::GetFirstIndexIdx() const {
	return m_firstIndex;
}

USize32 GpuMesh3D::GetNumIndices() const {
	return m_numIndices;
}

const COLLISION::SphereCollider& GpuMesh3D::GetBounds() const {
	return m_sphere;
}

COLLISION::SphereCollider& GpuMesh3D::GetBounds() {
	return m_sphere;
}

const Vector3f& GpuMesh3D::GetSphereCenter() const {
	return m_sphereCenter;
}

void GpuMesh3D::SetBoundingSphereRadius(float radius) {
	m_sphere.SetRadius(radius);
}

GpuMeshUuid GpuMesh3D::GetUuid() const {
	return m_uuid;
}

UIndex64 GpuMesh3D::GetMaterialIndex() const {
	return m_materialIndex;
}
