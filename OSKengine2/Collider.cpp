// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "Collider.h"

#include "Transform3D.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"
#include "ConvexVolume.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;


void Collider::CopyFrom(const Collider& other) {
	if (other.m_topLevelCollider.HasValue()) {
		m_topLevelCollider = other.m_topLevelCollider->CreateCopy().GetPointer();
	}

	m_bottomLevelColliders.Empty();

	for (const auto& blc : other.m_bottomLevelColliders)
		m_bottomLevelColliders.Insert(blc->CreateCopy().GetPointer());
}


void Collider::SetTopLevelCollider(OwnedPtr<ITopLevelCollider> collider) {
	m_topLevelCollider = collider.GetPointer();
}

void Collider::AddBottomLevelCollider(OwnedPtr<IBottomLevelCollider> collider) {
	m_bottomLevelColliders.Insert(collider.GetPointer());
}

CollisionInfo Collider::GetCollisionInfo(const Collider& other, const Transform3D& thisTransform, const Transform3D& otherTransform) const {
	const auto otherTopLevel = other.GetTopLevelCollider();
	if (!otherTopLevel)
		return CollisionInfo::False();

	const Vector3f thisPosition = thisTransform.GetPosition();
	const Vector3f otherPosition = otherTransform.GetPosition();

	if (!m_topLevelCollider->IsColliding(*otherTopLevel))
		return CollisionInfo::False();

	DynamicArray<DetailedCollisionInfo> bottomLevelCollisions{};

	for (const auto& bottomLevelA : m_bottomLevelColliders) {
		for (const auto& bottomLevelB : other.m_bottomLevelColliders) {

			const auto collisionInfo = bottomLevelA->GetCollisionInfo(*bottomLevelB.GetPointer(), thisTransform, otherTransform);
			
			if (collisionInfo.IsColliding()) {
				bottomLevelCollisions.Insert(collisionInfo);
			}
		}
	}

	return bottomLevelCollisions.IsEmpty()
		? CollisionInfo::TopLevelOnly()
		: CollisionInfo::True(bottomLevelCollisions);
}

USize32 Collider::GetBottomLevelCollidersCount() const {
	return static_cast<USize32>(m_bottomLevelColliders.GetSize());
}


template <>
nlohmann::json PERSISTENCE::SerializeJson<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data) {
	nlohmann::json output{};

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(data.GetTopLevelCollider())) {
		output["top_level_type"] = "AxisAlignedBoundingBox";
		output["m_topLevelCollider"] = SerializeJson<AxisAlignedBoundingBox>(*box);
	}
	else if (auto sphere = dynamic_cast<const SphereCollider*>(data.GetTopLevelCollider())) {
		output["top_level_type"] = "SphereCollider";
		output["m_topLevelCollider"] = SerializeJson<SphereCollider>(*sphere);
	}

	UIndex64 i = 0;
	for (const auto& blc : data.m_bottomLevelColliders) {
		output["m_bottomLevelColliders"][std::to_string(i)] = SerializeJson<ConvexVolume>(*blc->As<ConvexVolume>());
		i++;
	}

	return output;
}

template <>
OSK::COLLISION::Collider PERSISTENCE::DeserializeJson<OSK::COLLISION::Collider>(const nlohmann::json& json) {
	Collider output{};

	OwnedPtr<ITopLevelCollider> topLevelCollider = nullptr;

	if (json["top_level_type"] == "AxisAlignedBoundingBox") {
		topLevelCollider = DeserializeJson<AxisAlignedBoundingBox>(json["m_topLevelCollider"]).CreateCopy();
	}
	else if (json["top_level_type"] == "SphereCollider") {
		topLevelCollider = DeserializeJson<SphereCollider>(json["m_topLevelCollider"]).CreateCopy();
	}

	for (const auto& [key, value] : json["m_bottomLevelColliders"].items()) {
		const ConvexVolume& volume = DeserializeJson<ConvexVolume>(value);
		output.AddBottomLevelCollider(volume.CreateCopy());
	}

	return output;
}
