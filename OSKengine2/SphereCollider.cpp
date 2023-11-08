// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SphereCollider.h"

#include "AxisAlignedBoundingBox.h"

using namespace OSK;
using namespace OSK::COLLISION;

OwnedPtr<ITopLevelCollider> SphereCollider::CreateCopy() const {
	return new SphereCollider(*this);
}

SphereCollider::SphereCollider(float radius) {
	SetRadius(radius);
}

void SphereCollider::SetRadius(float radius) {
	this->radius = radius;
}

float SphereCollider::GetRadius() const {
	return radius;
}

bool SphereCollider::ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const {
	return point.GetDistanceTo(thisOffset) < radius;
}

bool SphereCollider::IsBehindPlane(Plane plane, const Vector3f& center) const {
	const float planeDistance = plane.point.Dot(plane.normal);
	const float centerDistance = center.Dot(plane.normal);

	const float distanceToPlane = centerDistance - planeDistance;

	return !(distanceToPlane > -radius);
}

RayCastResult SphereCollider::CastRay(const Ray& ray, const Vector3f& center) const {
	// https://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/

	// ray.origin -> position
	const Vector3f originsVec = center - ray.origin;
	
	// La esfera está detras del rayo
	if (ray.direction.Dot(originsVec))
		return RayCastResult::False();

	const float centerProjectionDistance = ray.direction.Dot(center);
	const Vector3f centerProjection = ray.origin + ray.direction * centerProjectionDistance;

	const float distance = centerProjection.GetDistanceTo(center);
	if (distance < radius)
		return RayCastResult::False();

	const float distanceToIntersection = centerProjection.GetDistanceTo(ray.origin)
		- glm::sqrt(radius * radius - distance * distance);

	return RayCastResult::True(ray.origin + ray.direction * distanceToIntersection);
}

bool SphereCollider::IsColliding(const ITopLevelCollider& other,
	const Vector3f& thisOffset, const Vector3f& otherOffset) const {

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(&other))
		return ITopLevelCollider::AabbSphereCollision(*box, *this,
			otherOffset, thisOffset);

	if (auto sphere = dynamic_cast<const SphereCollider*>(&other))
		return ITopLevelCollider::SphereSphereCollision(*this, *sphere,
			thisOffset, otherOffset);

	OSK_UNREACHABLE;
}
