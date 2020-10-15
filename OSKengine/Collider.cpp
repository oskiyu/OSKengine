#include "Collider.h"

using namespace OSK;
using namespace OSK::Collision;

Collider::Collider() {

}

Collider::Collider(const Vector3f& spherePosition, const float& sphereRadius) {
	SetBroadCollider(CollisionSphere(spherePosition, sphereRadius));
}

Collider::Collider(const Vector3f& boxPosition, const Vector3f& boxSize) {
	SetBroadCollider(CollisionBox(boxPosition, boxSize));
}

void Collider::SetBroadCollider(const CollisionSphere& sphere) {
	type = BroadColliderType::SPHERE;
	BroadCollider.Sphere = sphere;
}

void Collider::SetBroadCollider(const CollisionBox& box) {
	type = BroadColliderType::BOX_AABB;
	BroadCollider.Box = box;
}

void Collider::SetPosition(const Vector3f& pos) {
	if (type == BroadColliderType::BOX_AABB)
		BroadCollider.Box.Position = pos;
	else
		BroadCollider.Sphere.Position = pos;
}

bool Collider::IsColliding(Collider& other) {
	return GetCollisionInfo(other).IsColliding;
}

ColliderCollisionInfo Collider::GetCollisionInfo(Collider& other) {
	ColliderCollisionInfo info{};

	if (this == &other)
		goto return_p;
	
	//BROAD PHASE.
	if (type == BroadColliderType::BOX_AABB) { //Este collider tiene un AABB box.
		BroadCollider.Box.Position = transform.GlobalPosition;

		if (other.type == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Box))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Sphere))
				goto return_p;
		}
	}
	else {//Este collider tiene una esfera.
		BroadCollider.Sphere.Position = transform.GlobalPosition;
		
		if (other.type == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Box))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Sphere))
				goto return_p;
		}
	}
	
	info.IsBroadColliderColliding = true;

	for (auto& i : OBBs) {
		i.TransformPoints();
		for (auto& j : other.OBBs) {
			j.TransformPoints();
			if (&i == &j)
				continue;

			if (i.Intersects(j)) {
				info.IsColliding = true;
				info.SAT_1 = &i;
				info.SAT_2 = (SAT_Collider*)&j;

				goto return_p;
			}
		}
	}

return_p:
	return info;
}