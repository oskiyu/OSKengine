#include "Collider.h"

using namespace OSK;
using namespace OSK::Collision;

Collider::Collider() {

}

Collider::Collider(const Vector3f& spherePosition, float sphereRadius) {
	SetBroadCollider(CollisionSphere(spherePosition, sphereRadius));
}

Collider::Collider(const Vector3f& boxPosition, const Vector3f& boxSize) {
	SetBroadCollider(CollisionBox(boxPosition, boxSize));
}

void Collider::SetBroadCollider(const CollisionSphere& sphere) {
	BroadType = BroadColliderType::SPHERE;
	BroadCollider.Sphere = sphere;
}

void Collider::SetBroadCollider(const CollisionBox& box) {
	BroadType = BroadColliderType::BOX_AABB;
	BroadCollider.Box = box;
}

void Collider::SetPosition(const Vector3f& pos) {
	if (BroadType == BroadColliderType::BOX_AABB)
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
	if (BroadType == BroadColliderType::BOX_AABB) { //Este collider tiene un AABB box.
		BroadCollider.Box.Position = ColliderTransform.GlobalPosition;

		if (other.BroadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Box))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Sphere))
				goto return_p;
		}
	}
	else {//Este collider tiene una esfera.
		BroadCollider.Sphere.Position = ColliderTransform.GlobalPosition;
		
		if (other.BroadType == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Box))
				goto return_p;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Sphere))
				goto return_p;
		}
	}
	
	info.IsBroadColliderColliding = true;

	for (auto& i : SatColliders) {
		i.TransformPoints();
		for (auto& j : other.SatColliders) {
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