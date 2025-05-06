#include "NarrowColliderHolder.h"

OSK::COLLISION::NarrowColliderHolder::NarrowColliderHolder(UniquePtr<INarrowCollider>&& collider)
	: m_collider(std::move(collider)) {
	m_collider->_SetNarrowOwner(this);
}

OSK::UniquePtr<OSK::COLLISION::NarrowColliderHolder> OSK::COLLISION::NarrowColliderHolder::CreateCopy() const {
	auto output = MakeUnique<NarrowColliderHolder>(m_collider->CreateNarrowCopy());
	output->Transform(m_transform);

	return output;
}

OSK::COLLISION::INarrowCollider* OSK::COLLISION::NarrowColliderHolder::GetCollider() {
	return m_collider.GetPointer();
}

const OSK::COLLISION::INarrowCollider* OSK::COLLISION::NarrowColliderHolder::GetCollider() const {
	return m_collider.GetPointer();
}

const OSK::Transform3D& OSK::COLLISION::NarrowColliderHolder::GetTransform() const {
	return m_transform;
}

void OSK::COLLISION::NarrowColliderHolder::Transform(const Transform3D& transform) {
	m_transform = transform;
	m_collider->OnTransform();
}
