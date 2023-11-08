// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "AxisAlignedBoundingBox.h"

#include "SphereCollider.h"
#include "Assert.h"
#include "NotImplementedException.h"
#include "OSKengine.h"

using namespace OSK;
using namespace OSK::COLLISION;

OwnedPtr<ITopLevelCollider> AxisAlignedBoundingBox::CreateCopy() const {
	return new AxisAlignedBoundingBox(*this);
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3f& size) {
	SetSize(size);
}

void AxisAlignedBoundingBox::SetSize(const Vector3f& size) {
	m_size = size;
}

const Vector3f& AxisAlignedBoundingBox::GetSize() const {
	return m_size;
}

bool AxisAlignedBoundingBox::ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const {
	const Vector3f min = GetMin(thisOffset);
	const Vector3f max = GetMax(thisOffset);

	return
		point.x > min.x && point.x < max.x &&
		point.y > min.y && point.y < max.y &&
		point.z > min.z && point.z < max.z;
}

RayCastResult AxisAlignedBoundingBox::CastRay(const Ray& ray, const Vector3f& position) const {
	OSK_ASSERT(false, NotImplementedException());
	return RayCastResult::False();
}

bool AxisAlignedBoundingBox::IsBehindPlane(Plane plane, const Vector3f& position) const {
	const Vector3f extent = m_size * 0.5f;

	const float collapsedAabb =
		extent.x * glm::abs(plane.normal.x) +
		extent.y * glm::abs(plane.normal.y) +
		extent.z * glm::abs(plane.normal.z);

	const float signedDistanceToPlane = plane.normal.Dot(position - plane.point);

	return !(glm::abs(signedDistanceToPlane) <= collapsedAabb);
}

bool AxisAlignedBoundingBox::IsColliding(const ITopLevelCollider& other,
	const Vector3f& thisOffset, const Vector3f& otherOffset) const {

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(&other))
		return ITopLevelCollider::AabbAabbCollision(*this, *box,
			thisOffset, otherOffset);

	if (auto sphere = dynamic_cast<const SphereCollider*>(&other))
		return ITopLevelCollider::AabbSphereCollision(*this, *sphere,
			thisOffset, otherOffset);

	OSK_ASSERT(false, NotImplementedException());
}

Vector3f AxisAlignedBoundingBox::GetMin(const Vector3f& position) const {
	return position - m_size * 0.5f;
}

Vector3f AxisAlignedBoundingBox::GetMax(const Vector3f& position) const {
	return position + m_size * 0.5f;
}
