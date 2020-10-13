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

bool Collider::IsColliding(const Collider& other) const {
	//BROAD PHASE.
	/*if (type == BroadColliderType::BOX_AABB) { //Este collider tiene un AABB box.
		if (other.type == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Box))
				return false;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Box.Intersects(other.BroadCollider.Sphere))
				return false;
		}
	}
	else {//Este collider tiene una esfera.
		if (other.type == BroadColliderType::BOX_AABB) {//El otro collider tiene un AABB box.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Box))
				return false;
		}
		else {//El otro collider tiene una esfera.
			if (!BroadCollider.Sphere.Intersects(other.BroadCollider.Sphere))
				return false;
		}
	}*/

	//for (const auto& i : OBBs)
		//for (const auto& j : other.OBBs)
			//if (i.Intersects(j))
				//return true;
	if (OBBs[0].Intersects(other.OBBs[0]))
		return true;

	return false;
}