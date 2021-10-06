#include "Collider.h"

using namespace OSK;
using namespace OSK::Collision;

Collider::Collider(const Vector3f& spherePosition, float sphereRadius) {
	SetBroadCollider(CollisionSphere(spherePosition, sphereRadius));
}

Collider::Collider(const Vector3f& boxPosition, const Vector3f& boxSize) {
	SetBroadCollider(CollisionBox(boxPosition, boxSize));
}

void Collider::SetBroadCollider(const CollisionSphere& sphere) {
	broadType = BroadColliderType::SPHERE;
	broadCollisionSphere = sphere;
	broadCollisionSphere.transform.AttachTo(&transform);
}

void Collider::SetBroadCollider(const CollisionBox& box) {
	broadType = BroadColliderType::BOX_AABB;
	broadCollisionBox = box;
	broadCollisionBox.transform.AttachTo(&transform);
}

bool Collider::IsColliding(Collider& other) {
	return GetCollisionInfo(other).isColliding;
}

BroadColliderType Collider::GetCurrentBroadColliderType() const {
	return broadType;
}

ColliderCollisionInfo Collider::GetCollisionInfo(Collider& other) {
	ColliderCollisionInfo info{};

	if (this == &other)
		goto return_p;
	
	//BROAD PHASE.
	if (broadType == BroadColliderType::BOX_AABB) { //Este collider tiene un AABB box.
		if (other.broadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!broadCollisionBox.Intersects(other.broadCollisionBox))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!broadCollisionBox.Intersects(other.broadCollisionSphere))
				goto return_p;
		}
	}
	else {//Este collider tiene una esfera.
		if (other.broadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!broadCollisionSphere.Intersects(other.broadCollisionBox))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!broadCollisionSphere.Intersects(other.broadCollisionSphere))
				goto return_p;
		}
	}
	
	info.isBroadColliderColliding = true;

	for (auto& i : satColliders) {
		i.TransformPoints();
		for (auto& j : other.satColliders) {
			j.TransformPoints();
			if (&i == &j)
				continue;

			if (i.Intersects(j)) {
				info.isColliding = true;
				info.sat1 = &i;
				info.sat2 = (SAT_Collider*)&j;

				goto return_p;
			}
		}
	}

return_p:
	return info;
}

CollisionBox& Collider::GetBroadCollisionBox() {
	return broadCollisionBox;
}

CollisionSphere& Collider::GetBroadCollisionSphere() {
	return broadCollisionSphere;
}

Transform* Collider::GetTransform() {
	return &transform;
}

void Collider::AddCollider(const Collision::SAT_Collider& collider) {
	satColliders.push_back(collider);
}
