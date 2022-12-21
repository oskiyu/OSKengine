// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ITopLevelCollider.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"

using namespace OSK;
using namespace OSK::COLLISION;

bool ITopLevelCollider::AabbAabbCollision(const AxisAlignedBoundingBox& first, const AxisAlignedBoundingBox& other,
	const Vector3f& positionA, const Vector3f& positionB) {

	return positionA.X <= other.GetMax(positionB).X && first.GetMax(positionA).X >= positionB.X
		&& positionA.Y <= other.GetMax(positionB).Y && first.GetMax(positionA).Y >= positionB.Y
		&& positionA.Z <= other.GetMax(positionB).Z && first.GetMax(positionA).Z >= positionB.Z;
}

bool ITopLevelCollider::AabbSphereCollision(const AxisAlignedBoundingBox& box, const SphereCollider& sphere,
	const Vector3f& positionA, const Vector3f& positionB) {

	// Punto de la caja más cercano a la esfera.
	const Vector3f point = {
		glm::max(positionA.X, glm::min(positionB.X, box.GetMax(positionA).X)),
		glm::max(positionA.Y, glm::min(positionB.Y, box.GetMax(positionA).Y)),
		glm::max(positionA.Z, glm::min(positionB.Z, box.GetMax(positionA).Z))
	};

	return sphere.ContainsPoint(point);
}

bool ITopLevelCollider::SphereSphereCollision(const SphereCollider& first, const SphereCollider& other,
	const Vector3f& positionA, const Vector3f& positionB) {

	const float distance2 = positionA.GetDistanceTo2(positionB);

	const float radiusA2 = first.GetRadius() * first.GetRadius();
	const float radiusB2 = other.GetRadius() * other.GetRadius();

	return distance2 < radiusA2 + radiusB2;
}
