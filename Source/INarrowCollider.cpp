#include "INarrowCollider.h"

void OSK::COLLISION::INarrowCollider::_SetNarrowOwner(const NarrowColliderHolder* owner) {
	m_narrowOwner = owner;
}

std::optional<const OSK::COLLISION::NarrowColliderHolder*> OSK::COLLISION::INarrowCollider::GetNarrowOwner() const {
	return m_narrowOwner;
}
