// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "AxisAlignedBoundingBox.h"

#include "SphereCollider.h"
#include "Assert.h"

using namespace OSK;
using namespace OSK::COLLISION;

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3f& size) {
	SetSize(size);
}

void AxisAlignedBoundingBox::SetSize(const Vector3f& size) {
	this->size = size;
}

const Vector3f& AxisAlignedBoundingBox::GetSize() const {
	return size;
}

bool AxisAlignedBoundingBox::ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const {
	const Vector3f min = GetMin(thisOffset);
	const Vector3f max = GetMax(thisOffset);

	return
		point.X > min.X && point.X < max.X &&
		point.Y > min.Y && point.Y < max.Y &&
		point.Z > min.Z && point.Z < max.Z;
}

RayCastResult AxisAlignedBoundingBox::CastRay(const Ray& ray, const Vector3f& position) const {
	OSK_ASSERT(false, "No implementado.");
	return RayCastResult::False();
}

bool AxisAlignedBoundingBox::IsColliding(const ITopLevelCollider& other,
	const Vector3f& thisOffset, const Vector3f& otherOffset) const {

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(&other))
		return ITopLevelCollider::AabbAabbCollision(*this, *box,
			thisOffset, otherOffset);

	if (auto sphere = dynamic_cast<const SphereCollider*>(&other))
		return ITopLevelCollider::AabbSphereCollision(*this, *sphere,
			thisOffset, otherOffset);

	OSK_ASSERT(false, "Registrar nuevo tipo.");
	OSK_ASSUME(0);
}

Vector3f AxisAlignedBoundingBox::GetMin(const Vector3f& position) const {
	return position - size * 0.5f;
}

Vector3f AxisAlignedBoundingBox::GetMax(const Vector3f& position) const {
	return position + size * 0.5f;
}
