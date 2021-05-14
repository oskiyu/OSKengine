#include "CollisionBox.h"
#include "CollisionSphere.h"
#include <algorithm>

namespace OSK {

	CollisionBox::CollisionBox() {
		size = { 1.0f };
	}

	CollisionBox::CollisionBox(const Vector3f& position, const Vector3f& size) {
		this->position = position;
		this->size = size;
	}

	bool CollisionBox::Intersects(const CollisionBox& box) const {
		return position.X <= box.GetMax().X && GetMax().X >= box.position.X
			&& position.Y <= box.GetMax().Y && GetMax().Y >= box.position.Y
			&& position.Z <= box.GetMax().Z && GetMax().Z >= box.position.Z;
	}

	bool CollisionBox::Intersects(const CollisionSphere& sphere) const  {
		//Punto de la caja más cercano a la esfera.
		float x = std::max(position.X, std::min(sphere.position.X, GetMax().X));
		float y = std::max(position.Y, std::min(sphere.position.Y, GetMax().Y));
		float z = std::max(position.Z, std::min(sphere.position.Z, GetMax().Z));

		return sphere.ContainsPoint({ x, y, z });
	}

	bool CollisionBox::ContainsPoint(const Vector3f& point) const {
		return point.X >= position.X && point.X <= GetMax().X
			&& point.Y >= position.Y && point.Y <= GetMax().Y
			&& point.Z >= position.Z && point.Z <= GetMax().Z;
	}

	Vector3f CollisionBox::GetMax() const {
		return position + size;
	}

	Vector3f CollisionBox::GetMin() const {
		return position - size;
	}

}