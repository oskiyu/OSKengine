#include "CollisionSphere.h"

#include "CollisionBox.h"

namespace OSK {
	
	CollisionSphere::CollisionSphere() {

	}

	CollisionSphere::CollisionSphere(const Vector3f& position, float radius) {
		Position = position;
		Radius = radius;
	}

	bool CollisionSphere::ContainsPoint(const Vector3f& point) const {
		const float distance = point.GetDistanceTo2(Position);

		return distance <= Radius * Radius;
	}

	bool CollisionSphere::Intersects(const CollisionSphere& sphere) const {
		const float distance = sphere.Position.GetDistanceTo2(Position);

		return distance < (Radius + sphere.Radius) * (Radius + sphere.Radius);
	}

	bool CollisionSphere::Intersects(const CollisionBox& box) const {
		return box.Intersects(*this);
	}

}