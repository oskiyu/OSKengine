#include "CollisionSphere.h"

#include "CollisionBox.h"

namespace OSK {
	
	CollisionSphere::CollisionSphere() {

	}

	CollisionSphere::CollisionSphere(const Vector3f& position, float radius) {
		this->position = position;
		this->radius = radius;
	}

	bool CollisionSphere::ContainsPoint(const Vector3f& point) const {
		const float distance = point.GetDistanceTo2(position);

		return distance <= radius * radius;
	}

	bool CollisionSphere::Intersects(const CollisionSphere& sphere) const {
		const float distance = sphere.position.GetDistanceTo2(position);

		return distance < (radius + sphere.radius) * (radius + sphere.radius);
	}

	bool CollisionSphere::Intersects(const CollisionBox& box) const {
		return box.Intersects(*this);
	}

}