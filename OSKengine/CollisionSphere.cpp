#include "CollisionSphere.h"

#include "CollisionBox.h"

namespace OSK {
	
	CollisionSphere::CollisionSphere() {

	}

	CollisionSphere::CollisionSphere(const Vector3f& position, float radius) {
		transform.SetPosition(position);

		this->radius = radius;
	}

	bool CollisionSphere::ContainsPoint(const Vector3f& point) const {
		const float distance = point.GetDistanceTo2(transform.GetPosition());

		return distance <= radius * radius;
	}

	bool CollisionSphere::Intersects(const CollisionSphere& sphere) const {
		const float distance = sphere.transform.GetPosition().GetDistanceTo2(transform.GetPosition());

		return distance < (radius + sphere.radius) * (radius + sphere.radius);
	}

	bool CollisionSphere::Intersects(const CollisionBox& box) const {
		return box.Intersects(*this);
	}

}