// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "Collider.h"

#include "Transform3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;


void Collider::SetTopLevelCollider(OwnedPtr<ITopLevelCollider> collider) {
	topLevelCollider = collider.GetPointer();
}

void Collider::AddBottomLevelCollider(OwnedPtr<IBottomLevelCollider> collider) {
	bottomLevelColliders.Insert(collider.GetPointer());
}

CollisionInfo Collider::GetCollisionInfo(const Collider& other, const Transform3D& thisTransform, const Transform3D& otherTransform) const {
	const auto otherTopLevel = other.GetTopLevelCollider();
	if (!otherTopLevel)
		return CollisionInfo::False();

	const Vector3f thisPosition = thisTransform.GetPosition();
	const Vector3f otherPosition = otherTransform.GetPosition();

	if (!this->topLevelCollider->IsColliding(*otherTopLevel, thisPosition, otherPosition))
		return CollisionInfo::False();

	for (const auto& bottomLevelA : this->bottomLevelColliders) {
		for (const auto& bottomLevelB : other.bottomLevelColliders) {

			const auto collisionInfo = bottomLevelA->GetCollisionInfo(*bottomLevelB.GetPointer(), thisTransform, otherTransform);
			if (collisionInfo.IsColliding())
				return CollisionInfo::True(collisionInfo);
		}
	}

	return CollisionInfo::TopLevelOnly();
}

USize32 Collider::GetBottomLevelCollidersCount() const {
	return static_cast<USize32>(bottomLevelColliders.GetSize());
}
