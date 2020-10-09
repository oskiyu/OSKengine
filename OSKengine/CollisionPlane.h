#pragma once

#include "OSKtypes.h"

#include "CollisionSphere.h"
#include "CollisionInfo.h"

namespace OSK {

	struct CollisionPlane {

		Vector3f Normal;
		float Distance;

		inline bool Intersects(const CollisionSphere& sphere) const {
			const float distanceFromSphere = std::abs(Normal.Dot(sphere.Position) + Distance) - sphere.Radius;

			return distanceFromSphere < 0;
		}

	};

}
