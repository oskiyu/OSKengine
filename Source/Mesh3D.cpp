#include "Mesh3D.h"

using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

Mesh3D::Mesh3D(USize32 numIndices, UIndex32 firstIndex, const Vector3f& sphereCenter, UIndex64 id)
	: m_meshId(id), numIndices(numIndices), firstIndex(firstIndex), sphereCenter(sphereCenter) {

}

UIndex64 Mesh3D::GetMeshId() const {
	return m_meshId;
}

void Mesh3D::SetBoundingSphereRadius(float radius) {
	sphere = SphereCollider(radius);
}

const SphereCollider& Mesh3D::GetBounds() const {
	return sphere;
}

SphereCollider& Mesh3D::GetBounds() {
	return sphere;
}

const OSK::Vector3f& Mesh3D::GetSphereCenter() const {
	return sphereCenter;
}

USize32 Mesh3D::GetNumberOfIndices() const {
	return numIndices;
}

UIndex32 Mesh3D::GetFirstIndexId() const {
	return firstIndex;
}
