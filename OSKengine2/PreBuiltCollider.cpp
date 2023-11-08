#include "PreBuiltCollider.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::COLLISION;

PreBuiltCollider::PreBuiltCollider(const std::string& assetFile) : IAsset(assetFile) {}

void PreBuiltCollider::_SetCollider(OwnedPtr<COLLISION::Collider> collider) {
	m_collider = collider.GetPointer();
}

const COLLISION::Collider* PreBuiltCollider::GetCollider() const {
	return m_collider.GetPointer();
}
