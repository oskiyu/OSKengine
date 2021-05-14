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
	broadCollider.asSphere = sphere;
}

void Collider::SetBroadCollider(const CollisionBox& box) {
	broadType = BroadColliderType::BOX_AABB;
	broadCollider.asBox = box;
}

void Collider::SetPosition(const Vector3f& pos) {
	if (broadType == BroadColliderType::BOX_AABB)
		broadCollider.asBox.position = pos;
	else
		broadCollider.asSphere.position = pos;
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
		broadCollider.asBox.position = transform.GetPosition();

		if (other.broadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!broadCollider.asBox.Intersects(other.broadCollider.asBox))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!broadCollider.asBox.Intersects(other.broadCollider.asSphere))
				goto return_p;
		}
	}
	else {//Este collider tiene una esfera.
		broadCollider.asSphere.position = transform.GetPosition();
		
		if (other.broadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!broadCollider.asSphere.Intersects(other.broadCollider.asBox))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!broadCollider.asSphere.Intersects(other.broadCollider.asSphere))
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
	return broadCollider.asBox;
}

CollisionSphere& Collider::GetBroadCollisionSphere() {
	return broadCollider.asSphere;
}

Transform& Collider::GetTransform() {
	return transform;
}

void Collider::AddCollider(const Collision::SAT_Collider& collider) {
	satColliders.push_back(collider);
}
