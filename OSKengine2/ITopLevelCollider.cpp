// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ITopLevelCollider.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"

using namespace OSK;
using namespace OSK::COLLISION;

bool ITopLevelCollider::AabbAabbCollision(const AxisAlignedBoundingBox& first, const AxisAlignedBoundingBox& other) {

	const Vector3f firstMin = first.GetMin();
	const Vector3f firstMax = first.GetMax();

	const Vector3f secondMin = other.GetMin();
	const Vector3f secondMax = other.GetMax();

	return first.GetPosition().x <= other.GetMax().x && first.GetMax().x >= other.GetPosition().x
		&& first.GetPosition().y <= other.GetMax().y && first.GetMax().y >= other.GetPosition().y
		&& first.GetPosition().z <= other.GetMax().z && first.GetMax().z >= other.GetPosition().z;
}

bool ITopLevelCollider::AabbSphereCollision(const AxisAlignedBoundingBox& box, const SphereCollider& sphere) {

	// Punto de la caja más cercano a la esfera.
	const Vector3f point = {
		glm::max(box.GetMin().x, glm::min(sphere.GetPosition().x, box.GetMax().x)),
		glm::max(box.GetMin().y, glm::min(sphere.GetPosition().y, box.GetMax().y)),
		glm::max(box.GetMin().z, glm::min(sphere.GetPosition().z, box.GetMax().z))
	};

	return sphere.ContainsPoint(point);
}

bool ITopLevelCollider::SphereSphereCollision(const SphereCollider& first, const SphereCollider& other) {

	// Optimización: elevamos al cuadrado toda la inecuación para
	// así evitar un cálculo de raíz cuadrada, que es mas lento que
	// calcular un número al cuadrado.
	//
	// distance < radioA + radioB
	//
	// distance^2 < (radioA + radioB)^2
	const float distance2 = first.GetPosition().GetDistanceTo2(other.GetPosition());

	const float sumRadius = first.GetRadius() + other.GetRadius();
	const float sumRadius2 = sumRadius * sumRadius;

	return distance2 < sumRadius2;
}

bool ITopLevelCollider::IsInsideFrustum(const AnyFrustum& frustum) const {
	for (const auto& plane : frustum)
		if (this->IsBehindPlane(plane))
			return false;
	
	return true;
}

void ITopLevelCollider::SetPosition(const Vector3f& position) {
	m_position = position;
}

const Vector3f& ITopLevelCollider::GetPosition() const {
	return m_position;
}
