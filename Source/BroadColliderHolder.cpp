#include "BroadColliderHolder.h"

OSK::COLLISION::BroadColliderHolder::BroadColliderHolder(UniquePtr<IBroadCollider>&& collider) 
: m_collider(std::move(collider)) {
	m_collider->_SetBroadOwner(this);
}

OSK::UniquePtr<OSK::COLLISION::BroadColliderHolder> OSK::COLLISION::BroadColliderHolder::CreateCopy() const {
	auto output = MakeUnique<BroadColliderHolder>(m_collider->CreateBroadCopy());
	output->UpdatePosition(m_position);

	return output;
}

OSK::COLLISION::IBroadCollider* OSK::COLLISION::BroadColliderHolder::GetCollider() {
	return m_collider.GetPointer();
}

const OSK::COLLISION::IBroadCollider* OSK::COLLISION::BroadColliderHolder::GetCollider() const {
	return m_collider.GetPointer();
}

const OSK::Vector3f& OSK::COLLISION::BroadColliderHolder::GetPosition() const {
	return m_position;
}

void OSK::COLLISION::BroadColliderHolder::UpdatePosition(const Vector3f& position) {
	m_position = position;
}
