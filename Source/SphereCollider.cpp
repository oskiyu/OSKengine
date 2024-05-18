// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SphereCollider.h"

#include "AxisAlignedBoundingBox.h"
#include "Math.h"


using namespace OSK;
using namespace OSK::COLLISION;
using namespace OSK::PERSISTENCE;

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

bool SphereCollider::ContainsPoint(const Vector3f& point) const {
	return point.GetDistanceTo(GetPosition()) < radius;
}

bool SphereCollider::IsBehindPlane(Plane plane) const {
	const float planeDistance = plane.point.Dot(plane.normal);
	const float centerDistance = GetPosition().Dot(plane.normal);

	const float distanceToPlane = centerDistance - planeDistance;

	return !(distanceToPlane > -radius);
}

RayCastResult SphereCollider::CastRay(const Ray& ray) const {
	// https://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/

	// ray.origin -> position
	const Vector3f originsVec = GetPosition() - ray.origin;
	
	// La esfera está detras del rayo
	if (ray.direction.Dot(originsVec) < 0.0f) {
		return RayCastResult::False();
	}

	const float centerProjectionDistance = ray.direction.Dot(GetPosition());
	const Vector3f centerProjection = ray.origin + ray.direction * centerProjectionDistance;

	const float distance = centerProjection.GetDistanceTo(GetPosition());
	if (distance < radius)
		return RayCastResult::False();

	const float distanceToIntersection = centerProjection.GetDistanceTo(ray.origin)
		- glm::sqrt(radius * radius - distance * distance);

	return RayCastResult::True(ray.origin + ray.direction * distanceToIntersection, ECS::EMPTY_GAME_OBJECT);
}

bool SphereCollider::IsColliding(const ITopLevelCollider& other) const {
	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(&other))
		return ITopLevelCollider::AabbSphereCollision(*box, *this);

	if (auto sphere = dynamic_cast<const SphereCollider*>(&other))
		return ITopLevelCollider::SphereSphereCollision(*this, *sphere);
}

template <>
nlohmann::json PERSISTENCE::SerializeData<OSK::COLLISION::SphereCollider>(const OSK::COLLISION::SphereCollider& data) {
	nlohmann::json output{};

	output["m_radius"] = data.GetRadius();

	return output;
}

template <>
OSK::COLLISION::SphereCollider PERSISTENCE::DeserializeData<OSK::COLLISION::SphereCollider>(const nlohmann::json& json) {
	return SphereCollider(static_cast<float>(json["m_radius"]));
}



template <>
BinaryBlock PERSISTENCE::BinarySerializeData<OSK::COLLISION::SphereCollider>(const OSK::COLLISION::SphereCollider& data) {
	BinaryBlock output{};

	output.Write<float>(data.GetRadius());

	return output;
}

template <>
OSK::COLLISION::SphereCollider PERSISTENCE::BinaryDeserializeData<OSK::COLLISION::SphereCollider>(BinaryBlockReader* reader) {
	return SphereCollider(reader->Read<float>());
}
