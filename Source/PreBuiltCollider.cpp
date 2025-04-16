#include "PreBuiltCollider.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::COLLISION;

PreBuiltCollider::PreBuiltCollider(const std::string& assetFile) : IAsset(assetFile) {}

void PreBuiltCollider::_SetCollider(UniquePtr<COLLISION::Collider>&& collider) {
	m_collider = std::move(collider);
}

const COLLISION::Collider* PreBuiltCollider::GetCollider() const {
	return m_collider.GetPointer();
}
