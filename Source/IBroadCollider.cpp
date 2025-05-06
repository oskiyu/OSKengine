#include "IBroadCollider.h"


void OSK::COLLISION::IBroadCollider::_SetBroadOwner(const BroadColliderHolder* owner) {
	m_broadOwner = owner;
}

std::optional<const OSK::COLLISION::BroadColliderHolder*> OSK::COLLISION::IBroadCollider::GetBroadOwner() const {
	return m_broadOwner;
}
