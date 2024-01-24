#include "CollisionComponent.h"

#include "OSKengine.h"
#include "AssetManager.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::COLLISION;


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
nlohmann::json PERSISTENCE::SerializeJson<OSK::ECS::CollisionComponent>(const OSK::ECS::CollisionComponent& data) {
	nlohmann::json output{};

	if (data.m_preBuiltCollider.HasValue()) {
		output["m_preBuiltCollider"] = data.m_preBuiltCollider->GetAssetFilename();
	}

	if (data.m_customCollider.GetTopLevelCollider()) {
		output["m_customCollider"] = SerializeJson<Collider>(data.m_customCollider);
	}

	return output;
}

template <>
OSK::ECS::CollisionComponent PERSISTENCE::DeserializeJson<OSK::ECS::CollisionComponent>(const nlohmann::json& json) {
	OSK::ECS::CollisionComponent output{};

	if (json.contains("m_preBuiltCollider")) {
		output.m_preBuiltCollider = OSK::Engine::GetAssetManager()->Load<ASSETS::PreBuiltCollider>(json["m_preBuiltCollider"]);
	}

	if (json.contains("m_customCollider")) {
		output.m_customCollider = DeserializeJson<Collider>(json["m_customCollider"]);
	}

	return output;
}
