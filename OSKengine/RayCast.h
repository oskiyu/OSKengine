#pragma once

#include "OSKtypes.h"

#include <gtc/matrix_access.hpp>
#include "SAT_Collider.h"
#include "CollisionBox.h"

namespace OSK{

	struct RayCastCollisionInfo {
		bool IsColliding = false;
		float DistanceFromOrigin = { 0.0f };
	};

	class RayCast {

	public:

		static RayCastCollisionInfo CastRay(const Vector3f& origin, Vector3f direction, const Collision::SAT_Collider& sat);

	};

}
