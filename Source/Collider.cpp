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
using namespace OSK::PERSISTENCE;


void Collider::CopyFrom(const Collider& other) {
	if (other.m_topLevelCollider.HasValue()) {
		m_topLevelCollider = other.m_topLevelCollider->CreateCopy();
	}

	m_bottomLevelColliders.Empty();

	for (const auto& blc : other.m_bottomLevelColliders)
		m_bottomLevelColliders.Insert(blc->CreateCopy());
}


void Collider::SetTopLevelCollider(UniquePtr<ITopLevelCollider>&& collider) {
	m_topLevelCollider = std::move(collider);
}

void Collider::AddBottomLevelCollider(UniquePtr<IBottomLevelCollider>&& collider) {
	m_bottomLevelColliders.Insert(std::move(collider));
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
nlohmann::json PERSISTENCE::SerializeData<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data) {
	nlohmann::json output{};

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(data.GetTopLevelCollider())) {
		output["top_level_type"] = "AxisAlignedBoundingBox";
		output["m_topLevelCollider"] = SerializeData<AxisAlignedBoundingBox>(*box);
	}
	else if (auto sphere = dynamic_cast<const SphereCollider*>(data.GetTopLevelCollider())) {
		output["top_level_type"] = "SphereCollider";
		output["m_topLevelCollider"] = SerializeData<SphereCollider>(*sphere);
	}

	UIndex64 i = 0;
	for (const auto& blc : data.m_bottomLevelColliders) {
		output["m_bottomLevelColliders"][std::to_string(i)] = SerializeData<ConvexVolume>(*blc->As<ConvexVolume>());
		i++;
	}

	return output;
}

template <>
OSK::COLLISION::Collider PERSISTENCE::DeserializeData<OSK::COLLISION::Collider>(const nlohmann::json& json) {
	Collider output{};

	UniquePtr<ITopLevelCollider> topLevelCollider;

	if (json["top_level_type"] == "AxisAlignedBoundingBox") {
		topLevelCollider = DeserializeData<AxisAlignedBoundingBox>(json["m_topLevelCollider"]).CreateCopy();
	}
	else if (json["top_level_type"] == "SphereCollider") {
		topLevelCollider = DeserializeData<SphereCollider>(json["m_topLevelCollider"]).CreateCopy();
	}

	output.SetTopLevelCollider(std::move(topLevelCollider));

	for (const auto& [key, value] : json["m_bottomLevelColliders"].items()) {
		const ConvexVolume& volume = DeserializeData<ConvexVolume>(value);
		output.AddBottomLevelCollider(volume.CreateCopy());
	}

	return output;
}



template <>
BinaryBlock PERSISTENCE::BinarySerializeData<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data) {
	BinaryBlock output{};

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(data.GetTopLevelCollider())) {
		output.Write<TByte>(0);
		output.AppendBlock(BinarySerializeData<AxisAlignedBoundingBox>(*box));
	}
	else if (auto sphere = dynamic_cast<const SphereCollider*>(data.GetTopLevelCollider())) {
		output.Write<TByte>(1);
		output.AppendBlock(BinarySerializeData<SphereCollider>(*sphere));
	}

	output.Write<USize64>(data.m_bottomLevelColliders.GetSize());
	for (const auto& blc : data.m_bottomLevelColliders) {
		output.AppendBlock(BinarySerializeData<ConvexVolume>(*blc->As<ConvexVolume>()));
	}

	return output;
}

template <>
OSK::COLLISION::Collider PERSISTENCE::BinaryDeserializeData<OSK::COLLISION::Collider>(BinaryBlockReader* reader) {
	Collider output{};

	UniquePtr<ITopLevelCollider> topLevelCollider;

	const TByte type = reader->Read<TByte>();

	if (type == 0) {
		topLevelCollider = BinaryDeserializeData<AxisAlignedBoundingBox>(reader).CreateCopy();
	}
	else if (type == 1) {
		topLevelCollider = BinaryDeserializeData<SphereCollider>(reader).CreateCopy();
	}

	output.SetTopLevelCollider(std::move(topLevelCollider));

	const USize64 numBlc = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numBlc; i++) {
		const ConvexVolume& volume = BinaryDeserializeData<ConvexVolume>(reader);
		output.AddBottomLevelCollider(volume.CreateCopy());
	}

	return output;
}
