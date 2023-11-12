#include "CollisionComponent.h"

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
