#include "CollisionBox.h"
#include "CollisionSphere.h"
#include <algorithm>

namespace OSK {

	CollisionBox::CollisionBox() {
		transform.SetPosition(0.0f);
		transform.SetScale(1.0f);
	}

	CollisionBox::CollisionBox(const Vector3f& position, const Vector3f& size) {
		transform.SetPosition(position);
		transform.SetScale(size);
	}

	bool CollisionBox::Intersects(const CollisionBox& box) const {
		return transform.GetPosition().X <= box.GetMax().X && GetMax().X >= box.transform.GetPosition().X
			&& transform.GetPosition().Y <= box.GetMax().Y && GetMax().Y >= box.transform.GetPosition().Y
			&& transform.GetPosition().Z <= box.GetMax().Z && GetMax().Z >= box.transform.GetPosition().Z;
	}

	bool CollisionBox::Intersects(const CollisionSphere& sphere) const  {
		//Punto de la caja más cercano a la esfera.
		float x = std::max(transform.GetPosition().X, std::min(sphere.transform.GetPosition().X, GetMax().X));
		float y = std::max(transform.GetPosition().Y, std::min(sphere.transform.GetPosition().Y, GetMax().Y));
		float z = std::max(transform.GetPosition().Z, std::min(sphere.transform.GetPosition().Z, GetMax().Z));

		return sphere.ContainsPoint({ x, y, z });
	}

	bool CollisionBox::ContainsPoint(const Vector3f& point) const {
		return point.X >= transform.GetPosition().X && point.X <= GetMax().X
			&& point.Y >= transform.GetPosition().Y && point.Y <= GetMax().Y
			&& point.Z >= transform.GetPosition().Z && point.Z <= GetMax().Z;
	}

	Vector3f CollisionBox::GetMax() const {
		return transform.GetPosition() + transform.GetScale();
	}

	Vector3f CollisionBox::GetMin() const {
		return transform.GetPosition() - transform.GetScale();
	}

}