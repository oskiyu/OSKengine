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

	for (const auto& blc : other.m_bottomLevelColliders) {
		m_bottomLevelColliders.Insert(blc->CreateCopy());
	}
}


void Collider::SetTopLevelCollider(UniquePtr<BroadColliderHolder>&& collider) {
	m_topLevelCollider = std::move(collider);
}

void Collider::AddBottomLevelCollider(UniquePtr<NarrowColliderHolder>&& collider) {
	m_bottomLevelColliders.Insert(std::move(collider));
}

USize32 Collider::GetBottomLevelCollidersCount() const {
	return static_cast<USize32>(m_bottomLevelColliders.GetSize());
}


template <>
nlohmann::json PERSISTENCE::SerializeData<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data) {
	nlohmann::json output{};

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(data.GetTopLevelCollider()->GetCollider())) {
		output["top_level_type"] = "AxisAlignedBoundingBox";
		output["m_topLevelCollider"] = SerializeData<AxisAlignedBoundingBox>(*box);
	}
	else if (auto sphere = dynamic_cast<const SphereCollider*>(data.GetTopLevelCollider()->GetCollider())) {
		output["top_level_type"] = "SphereCollider";
		output["m_topLevelCollider"] = SerializeData<SphereCollider>(*sphere);
	}

	UIndex64 i = 0;
	for (const auto& blc : data.m_bottomLevelColliders) {
		output["m_bottomLevelColliders"][std::to_string(i)] = SerializeData<ConvexVolume>(*blc->GetCollider()->As<ConvexVolume>());
		i++;
	}

	return output;
}

template <>
OSK::COLLISION::Collider PERSISTENCE::DeserializeData<OSK::COLLISION::Collider>(const nlohmann::json& json) {
	Collider output{};

	UniquePtr<IBroadCollider> topLevelCollider;

	if (json["top_level_type"] == "AxisAlignedBoundingBox") {
		topLevelCollider = DeserializeData<AxisAlignedBoundingBox>(json["m_topLevelCollider"]).CreateBroadCopy();
	}
	else if (json["top_level_type"] == "SphereCollider") {
		topLevelCollider = DeserializeData<SphereCollider>(json["m_topLevelCollider"]).CreateBroadCopy();
	}

	output.SetTopLevelCollider(AsBroad(std::move(topLevelCollider)));

	for (const auto& [key, value] : json["m_bottomLevelColliders"].items()) {
		const ConvexVolume& volume = DeserializeData<ConvexVolume>(value);
		output.AddBottomLevelCollider(AsNarrow(volume.CreateNarrowCopy()));
	}

	return output;
}



template <>
BinaryBlock PERSISTENCE::BinarySerializeData<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data) {
	BinaryBlock output{};

	if (auto box = dynamic_cast<const AxisAlignedBoundingBox*>(data.GetTopLevelCollider()->GetCollider())) {
		output.Write<TByte>(0);
		output.AppendBlock(BinarySerializeData<AxisAlignedBoundingBox>(*box));
	}
	else if (auto sphere = dynamic_cast<const SphereCollider*>(data.GetTopLevelCollider()->GetCollider())) {
		output.Write<TByte>(1);
		output.AppendBlock(BinarySerializeData<SphereCollider>(*sphere));
	}

	output.Write<USize64>(data.m_bottomLevelColliders.GetSize());
	for (const auto& blc : data.m_bottomLevelColliders) {
		output.AppendBlock(BinarySerializeData<ConvexVolume>(*blc->GetCollider()->As<ConvexVolume>()));
	}

	return output;
}

template <>
OSK::COLLISION::Collider PERSISTENCE::BinaryDeserializeData<OSK::COLLISION::Collider>(BinaryBlockReader* reader) {
	Collider output{};

	UniquePtr<IBroadCollider> topLevelCollider;

	const TByte type = reader->Read<TByte>();

	if (type == 0) {
		topLevelCollider = BinaryDeserializeData<AxisAlignedBoundingBox>(reader).CreateBroadCopy();
	}
	else if (type == 1) {
		topLevelCollider = BinaryDeserializeData<SphereCollider>(reader).CreateBroadCopy();
	}

	output.SetTopLevelCollider(AsBroad(std::move(topLevelCollider)));

	const USize64 numBlc = reader->Read<USize64>();
	for (UIndex64 i = 0; i < numBlc; i++) {
		const ConvexVolume& volume = BinaryDeserializeData<ConvexVolume>(reader);
		output.AddBottomLevelCollider(AsNarrow(volume.CreateNarrowCopy()));
	}

	return output;
}
