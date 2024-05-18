#include "CollisionComponent.h"

#include "OSKengine.h"
#include "AssetManager.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::COLLISION;
using namespace OSK::PERSISTENCE;


void CollisionComponent::SetCollider(AssetRef<PreBuiltCollider> preBuiltCollider) {
	m_preBuiltCollider = preBuiltCollider;
	m_customCollider.CopyFrom(*m_preBuiltCollider->GetCollider());
}

void CollisionComponent::SetCollider(const Collider& collider) {
	m_customCollider.CopyFrom(collider);
}

Collider* CollisionComponent::GetCollider() {
	return &m_customCollider;
}

const Collider* CollisionComponent::GetCollider() const {
	return &m_customCollider;
}


template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::ECS::CollisionComponent>(const OSK::ECS::CollisionComponent& data) {
	nlohmann::json output{};

	if (data.m_preBuiltCollider.HasValue()) {
		output["m_preBuiltCollider"] = data.m_preBuiltCollider->GetAssetFilename();
	}

	if (data.m_customCollider.GetTopLevelCollider()) {
		output["m_customCollider"] = SerializeData<Collider>(data.m_customCollider);
	}

	return output;
}

template <>
OSK::ECS::CollisionComponent PERSISTENCE::DeserializeComponent<OSK::ECS::CollisionComponent>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	OSK::ECS::CollisionComponent output{};

	if (json.contains("m_preBuiltCollider")) {
		output.m_preBuiltCollider = OSK::Engine::GetAssetManager()->Load<ASSETS::PreBuiltCollider>(json["m_preBuiltCollider"]);
	}

	if (json.contains("m_customCollider")) {
		output.m_customCollider = DeserializeData<Collider>(json["m_customCollider"]);
	}

	return output;
}


template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::ECS::CollisionComponent>(const OSK::ECS::CollisionComponent& data) {
	BinaryBlock output{};

	output.Write<TByte>(data.m_preBuiltCollider.HasValue());
	output.Write<TByte>(static_cast<bool>(data.m_customCollider.GetTopLevelCollider()));

	if (data.m_preBuiltCollider.HasValue()) {
		output.WriteString(data.m_preBuiltCollider->GetAssetFilename());
	}

	if (data.m_customCollider.GetTopLevelCollider()) {
		output.AppendBlock(BinarySerializeData(data.m_customCollider));
	}

	return output;
}

template <>
OSK::ECS::CollisionComponent PERSISTENCE::BinaryDeserializeComponent<OSK::ECS::CollisionComponent>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	OSK::ECS::CollisionComponent output{};

	const bool hasPreBuiltCollider = reader->Read<TByte>();
	const bool hasCustomCollider = reader->Read<TByte>();

	if (hasPreBuiltCollider) {
		output.m_preBuiltCollider = OSK::Engine::GetAssetManager()->Load<ASSETS::PreBuiltCollider>(reader->ReadString());
	}

	if (hasCustomCollider) {
		output.m_customCollider = BinaryDeserializeData<Collider>(reader);
	}

	return output;
}
