// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ITopLevelCollider.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"

using namespace OSK;
using namespace OSK::COLLISION;

bool ITopLevelCollider::AabbAabbCollision(const AxisAlignedBoundingBox& first, const AxisAlignedBoundingBox& other,
	const Vector3f& positionA, const Vector3f& positionB) {

	const Vector3f firstMin = first.GetMin(positionA);
	const Vector3f firstMax = first.GetMax(positionA);

	const Vector3f secondMin = other.GetMin(positionA);
	const Vector3f secondMax = other.GetMax(positionA);

	return positionA.x <= other.GetMax(positionB).x && first.GetMax(positionA).x >= positionB.x
		&& positionA.y <= other.GetMax(positionB).y && first.GetMax(positionA).y >= positionB.y
		&& positionA.Z <= other.GetMax(positionB).Z && first.GetMax(positionA).Z >= positionB.Z;
}

bool ITopLevelCollider::AabbSphereCollision(const AxisAlignedBoundingBox& box, const SphereCollider& sphere,
	const Vector3f& boxPos, const Vector3f& spherePos) {

	// Punto de la caja más cercano a la esfera.
	const Vector3f point = {
		glm::max(box.GetMin(boxPos).x, glm::min(spherePos.x, box.GetMax(boxPos).x)),
		glm::max(box.GetMin(boxPos).y, glm::min(spherePos.y, box.GetMax(boxPos).y)),
		glm::max(box.GetMin(boxPos).Z, glm::min(spherePos.Z, box.GetMax(boxPos).Z))
	};

	return sphere.ContainsPoint(spherePos, point);
}

bool ITopLevelCollider::SphereSphereCollision(const SphereCollider& first, const SphereCollider& other,
	const Vector3f& positionA, const Vector3f& positionB) {

	// Optimización: elevamos al cuadrado toda la inecuación para
	// así evitar un cálculo de raíz cuadrada, que es mas lento que
	// calcular un número al cuadrado.
	//
	// distance < radioA + radioB
	//
	// distance^2 < (radioA + radioB)^2
	const float distance2 = positionA.GetDistanceTo2(positionB);

	const float sumRadius = first.GetRadius() + other.GetRadius();
	const float sumRadius2 = sumRadius * sumRadius;

	return distance2 < sumRadius2;
}
