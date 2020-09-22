#include "CollisionBox.h"
#include "CollisionSphere.h"
#include <algorithm>

namespace OSK {

	CollisionBox::CollisionBox() {

	}

	CollisionBox::CollisionBox(const Vector3f& position, const Vector3f& size) {
		Position = position;
		Size = size;
	}

	bool CollisionBox::Intersects(const CollisionBox& box) const {
		return Position.X <= box.GetMax().X && GetMax().X >= box.Position.X
			&& Position.Y <= box.GetMax().Y && GetMax().Y >= box.Position.Y
			&& Position.Z <= box.GetMax().Z && GetMax().Z >= box.Position.Z;
	}

	bool CollisionBox::Intersects(const CollisionSphere& sphere) const  {
		//Punto de la caja más cercano a la esfera.
		float x = std::max(Position.X, std::min(sphere.Position.X, GetMax().X));
		float y = std::max(Position.Y, std::min(sphere.Position.Y, GetMax().Y));
		float z = std::max(Position.Z, std::min(sphere.Position.Z, GetMax().Z));

		return sphere.ContainsPoint({ x, y, z });
	}

	bool CollisionBox::ContainsPoint(const Vector3f& point) const {
		return point.X >= Position.X && point.X <= GetMax().X
			&& point.Y >= Position.Y && point.Y <= GetMax().Y
			&& point.Z >= Position.Z && point.Z <= GetMax().Z;
	}

	Vector3f CollisionBox::GetMax() const {
		return Position + Size;
	}

}