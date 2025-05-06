#include "SphereCollider.h"

#include "UniquePtr.hpp"
#include "Vector3.hpp"

#include "BroadColliderHolder.h"
#include "NarrowColliderHolder.h"

#include "Plane.h"
#include "RayCastResult.h"
#include "Ray.h"

// Error handling.
#include "Assert.h"
#include "UnreachableException.h"

// Para GameObject::EMPTY.
#include "GameObject.h"

// Para glm::sqrt.
#include <glm/glm.hpp>

// Para serialización.
#include "BinaryBlock.h"
#include <json.hpp>

using namespace OSK;
using namespace OSK::COLLISION;
using namespace OSK::PERSISTENCE;

UniquePtr<IBroadCollider> SphereCollider::CreateBroadCopy() const {
	return MakeUnique<SphereCollider>(*this);
}

UniquePtr<INarrowCollider> SphereCollider::CreateNarrowCopy() const {
	return MakeUnique<SphereCollider>(*this);
}

SphereCollider::SphereCollider(float radius) {
	SetRadius(radius);
}

void SphereCollider::SetRadius(float radius) {
	m_radius = radius;
}

float SphereCollider::GetRadius() const {
	return m_radius;
}

GjkSupport SphereCollider::GetSupport(const Vector3f& direction) const {
	Vector3f position = Vector3f::Zero;
	if (GetBroadOwner().has_value()) {
		position = GetBroadOwner().value()->GetPosition();
	}
	else {
		position = GetNarrowOwner().value()->GetTransform().GetPosition();
	}

	return { position + direction * m_radius, {} };
}

RayCastResult SphereCollider::CastRay(const Ray& ray) const {
	// https://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/

	// ray.origin -> position
	Vector3f originsVec = Vector3f::Zero;
	if (GetBroadOwner().has_value()) {
		originsVec = GetBroadOwner().value()->GetPosition();
	}
	else {
		originsVec = GetNarrowOwner().value()->GetTransform().GetPosition();
	}
	
	// La esfera está detras del rayo
	if (ray.direction.Dot(originsVec) < 0.0f) {
		return RayCastResult::False();
	}

	const float centerProjectionDistance = ray.direction.Dot(originsVec);
	const Vector3f centerProjection = ray.origin + ray.direction * centerProjectionDistance;

	const float distance = centerProjection.GetDistanceTo(originsVec);
	if (distance < m_radius)
		return RayCastResult::False();

	const float distanceToIntersection = centerProjection.GetDistanceTo(ray.origin)
		- glm::sqrt(m_radius * m_radius - distance * distance);

	return RayCastResult::True(ray.origin + ray.direction * distanceToIntersection, ECS::EMPTY_GAME_OBJECT);
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
