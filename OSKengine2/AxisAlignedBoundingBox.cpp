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

bool AxisAlignedBoundingBox::ContainsPoint(const Vector3f& point) const {
	const Vector3f min = GetMin();
	const Vector3f max = GetMax();

	return
		point.x > min.x && point.x < max.x &&
		point.y > min.y && point.y < max.y &&
		point.z > min.z && point.z < max.z;
}

RayCastResult AxisAlignedBoundingBox::CastRay(const Ray& ray) const {
	const std::array<Vector3f, 6> normals = {
		Vector3f(1.0f, 0.0f, 0.0f),
		Vector3f(-1.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 1.0f, 0.0f),
		Vector3f(0.0f, -1.0f, 0.0f),
		Vector3f(0.0f, 0.0f, 1.0f),
		Vector3f(0.0f, 0.0f, -1.0f)
	};

	const std::array<Vector3f, 6> points = {
		GetPosition() + normals[0] * m_size * 0.5f,
		GetPosition() + normals[1] * m_size * 0.5f,
		GetPosition() + normals[2] * m_size * 0.5f,
		GetPosition() + normals[3] * m_size * 0.5f,
		GetPosition() + normals[4] * m_size * 0.5f,
		GetPosition() + normals[5] * m_size * 0.5f
	};

	float closestDistance = std::numeric_limits<float>::max();
	Vector3f closestIntersection = Vector3f::Zero;
	bool intersection = false;
	for (UIndex64 i = 0; i < 6; i++) {
		const auto result = ray.IntersectionWithPlane(normals[i], points[i]);

		if (!result.Result())
			continue;

		intersection = true;

		const float nDistance = result.GetIntersectionPoint().GetDistanceTo2(ray.origin);
		if (nDistance < closestDistance) {
			closestDistance = nDistance;
			closestIntersection = result.GetIntersectionPoint();
		}
	}

	return intersection
		? RayCastResult::True(closestIntersection, ECS::EMPTY_GAME_OBJECT)
		: RayCastResult::False();
}

bool AxisAlignedBoundingBox::IsBehindPlane(Plane plane) const {
	const Vector3f extent = m_size * 0.5f;

	const float collapsedAabb =
		extent.x * glm::abs(plane.normal.x) +
		extent.y * glm::abs(plane.normal.y) +
		extent.z * glm::abs(plane.normal.z);

	const float signedDistanceToPlane = plane.normal.Dot(GetPosition() - plane.point);

	return !(glm::abs(signedDistanceToPlane) <= collapsedAabb);
}

bool AxisAlignedBoundingBox::IsColliding(const ITopLevelCollider& other) const {

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(&other))
		return ITopLevelCollider::AabbAabbCollision(*this, *box);

	if (auto sphere = dynamic_cast<const SphereCollider*>(&other))
		return ITopLevelCollider::AabbSphereCollision(*this, *sphere);

	OSK_ASSERT(false, NotImplementedException());
}

Vector3f AxisAlignedBoundingBox::GetMin() const {
	return GetPosition() - m_size * 0.5f;
}

Vector3f AxisAlignedBoundingBox::GetMax() const {
	return GetPosition() + m_size * 0.5f;
}
